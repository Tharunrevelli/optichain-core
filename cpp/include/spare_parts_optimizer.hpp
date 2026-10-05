#ifndef OPTICHAIN_SPARE_PARTS_OPTIMIZER_HPP
#define OPTICHAIN_SPARE_PARTS_OPTIMIZER_HPP

#include <string>
#include <vector>
#include <unordered_map>

namespace optichain {

struct SparePartItem {
    int partId;
    std::string sku;
    std::string category; // e.g. "Critical Avionics", "Hydraulics", "Sensors"
    double unitCost;
    double failureRatePerYear; // Poisson arrival parameter lambda
    double downtimeCostPerHour;
    int repairLeadTimeDays;
    int maxAllocation;

    SparePartItem(int id, std::string sku, std::string category, double unitCost,
                   double failureRate, double downtimeCost, int leadTimeDays, int maxAlloc = 10)
        : partId(id), sku(std::move(sku)), category(std::move(category)),
          unitCost(unitCost), failureRatePerYear(failureRate),
          downtimeCostPerHour(downtimeCost), repairLeadTimeDays(leadTimeDays),
          maxAllocation(maxAlloc) {}
};

struct OptimizationAllocation {
    int partId;
    std::string sku;
    int allocatedUnits;
    double expectedBackorders;
    double capitalInvestment;
};

struct OptimizerResult {
    double totalBudget;
    double spentBudget;
    double expectedSystemAvailability; // e.g. 99.4%
    double totalExpectedAnnualBackorderHours;
    std::vector<OptimizationAllocation> allocations;
    std::vector<std::pair<double, double>> paretoFrontier; // (Budget, Availability) pairs
    double executionTimeMs;
};

class SparePartsOptimizer {
public:
    SparePartsOptimizer() = default;

    void addItem(const SparePartItem& item);
    const std::vector<SparePartItem>& getItems() const { return items_; }

    // Marginal Allocation (Sherbrooke's METRIC greedy algorithm with priority queue)
    // O(K * log M) where K is number of allocation steps and M is number of part SKUs
    OptimizerResult optimizeMarginalAllocation(double capitalBudget);

    // Dynamic Programming knapsack solver for constrained capital allocation
    OptimizerResult optimizeDynamicProgramming(double capitalBudget, int budgetStep = 100);

private:
    std::vector<SparePartItem> items_;

    // Computes Poisson probability P(X = k) for mean mu
    static double poissonPMF(int k, double mu);
    // Computes expected backorders E[max(0, X - s)] for stock level s
    static double expectedBackorders(int stockLevel, double annualLambda, int leadTimeDays);
};

} // namespace optichain

#endif // OPTICHAIN_SPARE_PARTS_OPTIMIZER_HPP
