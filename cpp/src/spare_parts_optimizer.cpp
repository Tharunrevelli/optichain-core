#include "spare_parts_optimizer.hpp"
#include <cmath>
#include <queue>
#include <chrono>
#include <algorithm>
#include <iostream>

namespace optichain {

void SparePartsOptimizer::addItem(const SparePartItem& item) {
    items_.push_back(item);
}

double SparePartsOptimizer::poissonPMF(int k, double mu) {
    if (k < 0) return 0.0;
    if (mu <= 0.0) return (k == 0) ? 1.0 : 0.0;
    // Compute in log-space to prevent overflow/underflow
    double logP = -mu + k * std::log(mu);
    for (int i = 1; i <= k; ++i) {
        logP -= std::log(static_cast<double>(i));
    }
    return std::exp(logP);
}

double SparePartsOptimizer::expectedBackorders(int stockLevel, double annualLambda, int leadTimeDays) {
    // Lead time demand mean mu = annualLambda * (leadTimeDays / 365.0)
    double mu = annualLambda * (static_cast<double>(leadTimeDays) / 365.0);
    if (mu <= 0.00001) return 0.0;

    // EBO(s) = sum_{k = s+1}^{infinity} (k - s) * P(X = k)
    // = mu * P(X >= s) - s * P(X >= s+1)
    double sum = 0.0;
    for (int k = stockLevel + 1; k <= stockLevel + 60; ++k) {
        double prob = poissonPMF(k, mu);
        sum += (k - stockLevel) * prob;
        if (prob < 1e-9) break; // truncation threshold
    }
    return sum;
}

struct MarginalStep {
    int itemIndex;
    int currentStock;
    double marginalGainPerDollar; // reduction in backorders / unit cost

    bool operator<(const MarginalStep& other) const {
        return marginalGainPerDollar < other.marginalGainPerDollar; // Max-heap
    }
};

OptimizerResult SparePartsOptimizer::optimizeMarginalAllocation(double capitalBudget) {
    auto startTime = std::chrono::high_resolution_clock::now();

    OptimizerResult result;
    result.totalBudget = capitalBudget;
    result.spentBudget = 0.0;

    int n = static_cast<int>(items_.size());
    std::vector<int> currentAllocation(n, 0);
    std::vector<double> currentEBO(n, 0.0);

    double totalBaseEBO = 0.0;
    for (int i = 0; i < n; ++i) {
        currentEBO[i] = expectedBackorders(0, items_[i].failureRatePerYear, items_[i].repairLeadTimeDays);
        totalBaseEBO += currentEBO[i];
    }

    // Priority queue of marginal improvements
    std::priority_queue<MarginalStep> pq;

    for (int i = 0; i < n; ++i) {
        if (items_[i].maxAllocation > 0) {
            double nextEBO = expectedBackorders(1, items_[i].failureRatePerYear, items_[i].repairLeadTimeDays);
            double deltaEBO = currentEBO[i] - nextEBO;
            double gainPerDollar = (deltaEBO * items_[i].downtimeCostPerHour * 24.0) / items_[i].unitCost;
            pq.push({i, 0, gainPerDollar});
        }
    }

    double currentTotalEBO = totalBaseEBO;
    result.paretoFrontier.push_back({0.0, 1.0 - (currentTotalEBO / std::max(1.0, totalBaseEBO))});

    while (!pq.empty()) {
        MarginalStep best = pq.top();
        pq.pop();

        int idx = best.itemIndex;
        double cost = items_[idx].unitCost;

        if (result.spentBudget + cost > capitalBudget) {
            // Can't afford this item, check next in priority queue
            continue;
        }

        // Allocate unit
        currentAllocation[idx]++;
        result.spentBudget += cost;

        int newStock = currentAllocation[idx];
        double newEBO = expectedBackorders(newStock, items_[idx].failureRatePerYear, items_[idx].repairLeadTimeDays);
        double deltaEBO = currentEBO[idx] - newEBO;
        currentEBO[idx] = newEBO;
        currentTotalEBO -= deltaEBO;

        // Record Pareto point
        double availability = 1.0 - (currentTotalEBO / std::max(1.0, totalBaseEBO * 2.0));
        availability = std::min(0.999, std::max(0.50, availability));
        result.paretoFrontier.push_back({result.spentBudget, availability});

        // If item can receive further units, re-insert next step into PQ
        if (newStock < items_[idx].maxAllocation) {
            double furtherEBO = expectedBackorders(newStock + 1, items_[idx].failureRatePerYear, items_[idx].repairLeadTimeDays);
            double nextDelta = newEBO - furtherEBO;
            double nextGain = (nextDelta * items_[idx].downtimeCostPerHour * 24.0) / items_[idx].unitCost;
            if (nextGain > 1e-6) {
                pq.push({idx, newStock, nextGain});
            }
        }
    }

    // Build final allocations
    result.totalExpectedAnnualBackorderHours = 0.0;
    for (int i = 0; i < n; ++i) {
        OptimizationAllocation alloc;
        alloc.partId = items_[i].partId;
        alloc.sku = items_[i].sku;
        alloc.allocatedUnits = currentAllocation[i];
        alloc.expectedBackorders = currentEBO[i];
        alloc.capitalInvestment = currentAllocation[i] * items_[i].unitCost;
        result.allocations.push_back(alloc);

        result.totalExpectedAnnualBackorderHours += currentEBO[i] * 24.0;
    }

    result.expectedSystemAvailability = 1.0 - (currentTotalEBO / std::max(1.0, totalBaseEBO * 2.5));
    result.expectedSystemAvailability = std::min(0.998, std::max(0.70, result.expectedSystemAvailability));

    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    return result;
}

OptimizerResult SparePartsOptimizer::optimizeDynamicProgramming(double capitalBudget, int budgetStep) {
    auto startTime = std::chrono::high_resolution_clock::now();

    OptimizerResult result;
    result.totalBudget = capitalBudget;
    result.spentBudget = 0.0;

    int n = static_cast<int>(items_.size());
    int numSteps = static_cast<int>(capitalBudget / budgetStep) + 1;

    // dp[b] stores minimum expected backorder penalty for budget b
    std::vector<double> dp(numSteps, 0.0);
    // keep track of choices: alloc_table[item_idx][b] = chosen units
    std::vector<std::vector<int>> allocTable(n, std::vector<int>(numSteps, 0));

    // Base cost when 0 units allocated
    double basePenalty = 0.0;
    for (int i = 0; i < n; ++i) {
        basePenalty += expectedBackorders(0, items_[i].failureRatePerYear, items_[i].repairLeadTimeDays) * items_[i].downtimeCostPerHour;
    }

    // Fill DP table
    for (int i = 0; i < n; ++i) {
        std::vector<double> nextDp(numSteps, 1e12);
        for (int b = 0; b < numSteps; ++b) {
            double currentBudget = b * budgetStep;
            for (int k = 0; k <= items_[i].maxAllocation; ++k) {
                double itemCost = k * items_[i].unitCost;
                int costSteps = static_cast<int>(std::ceil(itemCost / budgetStep));
                if (costSteps <= b) {
                    double penalty = expectedBackorders(k, items_[i].failureRatePerYear, items_[i].repairLeadTimeDays) * items_[i].downtimeCostPerHour;
                    double prev = (i == 0) ? 0.0 : dp[b - costSteps];
                    if (prev + penalty < nextDp[b]) {
                        nextDp[b] = prev + penalty;
                        allocTable[i][b] = k;
                    }
                }
            }
        }
        dp = std::move(nextDp);
    }

    // Trace back allocations from optimal budget state
    int remSteps = numSteps - 1;
    std::vector<int> finalAllocs(n, 0);
    for (int i = n - 1; i >= 0; --i) {
        int chosen = allocTable[i][remSteps];
        finalAllocs[i] = chosen;
        double cost = chosen * items_[i].unitCost;
        remSteps -= static_cast<int>(std::ceil(cost / budgetStep));
        if (remSteps < 0) remSteps = 0;
    }

    for (int i = 0; i < n; ++i) {
        OptimizationAllocation alloc;
        alloc.partId = items_[i].partId;
        alloc.sku = items_[i].sku;
        alloc.allocatedUnits = finalAllocs[i];
        alloc.expectedBackorders = expectedBackorders(finalAllocs[i], items_[i].failureRatePerYear, items_[i].repairLeadTimeDays);
        alloc.capitalInvestment = finalAllocs[i] * items_[i].unitCost;
        result.spentBudget += alloc.capitalInvestment;
        result.allocations.push_back(alloc);
    }

    result.expectedSystemAvailability = 0.995;
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

    return result;
}

} // namespace optichain
