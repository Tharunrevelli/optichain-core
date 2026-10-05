#ifndef OPTICHAIN_INVENTORY_POLICY_HPP
#define OPTICHAIN_INVENTORY_POLICY_HPP

#include <cmath>
#include <string>
#include <algorithm>

namespace optichain {

struct PolicyConfig {
    int reorderPoint;   // s
    int orderUpToLevel; // S
    double targetFillRate; // e.g. 0.98 (98%)
    double safetyStock;

    PolicyConfig(int s = 10, int S = 30, double fillRate = 0.95, double ss = 5.0)
        : reorderPoint(s), orderUpToLevel(S), targetFillRate(fillRate), safetyStock(ss) {}
};

struct InventoryMetrics {
    int totalDemandUnits = 0;
    int fulfilledUnits = 0;
    int backorderedUnits = 0;
    int stockoutEvents = 0;
    int replenishmentOrdersPlaced = 0;
    
    double totalHoldingCost = 0.0;
    double totalOrderingCost = 0.0;
    double totalStockoutPenalty = 0.0;
    double totalCost = 0.0;
    
    double achievedFillRate() const {
        return totalDemandUnits > 0 ? static_cast<double>(fulfilledUnits) / totalDemandUnits : 1.0;
    }
    double averageDailyCost(int days) const {
        return days > 0 ? totalCost / days : 0.0;
    }
};

class InventoryPolicyEngine {
public:
    // Calculates Z-score given target cycle service level
    static double normalInverseCDF(double p);

    // Calculates analytical safety stock for lead time L and daily demand mean mu, variance var
    static double calculateSafetyStock(double meanDailyDemand, double stdDevDailyDemand,
                                       double leadTimeDays, double targetServiceLevel);

    // Calibrates (s, S) policy using Wagner-Whitin / Silver-Meal inspired economic order quantity
    static PolicyConfig calibratePolicy(double meanDailyDemand, double stdDevDailyDemand,
                                        double leadTimeDays, double holdingCostPerUnitDay,
                                        double fixedOrderCost, double unitPurchaseCost,
                                        double targetFillRate);
};

} // namespace optichain

#endif // OPTICHAIN_INVENTORY_POLICY_HPP
