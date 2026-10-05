#include "inventory_policy.hpp"
#include <cmath>
#include <stdexcept>

namespace optichain {

// Rational approximation for normal inverse CDF (Beasley-Springer-Moro algorithm)
double InventoryPolicyEngine::normalInverseCDF(double p) {
    if (p <= 0.0 || p >= 1.0) {
        if (p >= 0.9999) return 3.719;
        if (p <= 0.0001) return -3.719;
    }

    // Coefficients in rational approximations
    const double a[] = {
        -3.969683028665376e+01,  2.209460984245205e+02,
        -2.759285104469687e+02,  1.383577518672690e+02,
        -3.066479806614716e+01,  2.506628277459239e+00
    };

    const double b[] = {
        -5.447609879822406e+01,  1.615858368580409e+02,
        -1.556989798598866e+02,  6.680133088874600e+01,
        -1.328068155288572e+01
    };

    const double c[] = {
        -7.784894002430293e-03, -3.223964580411365e-01,
        -2.400758277161838e+00, -2.549732539343734e+00,
         4.374664141464968e+00,  2.938163982698783e+00
    };

    const double d[] = {
        7.784695709041462e-03,  3.224671290700398e-01,
        2.445134137142996e+00,  3.754408661907416e+00
    };

    double q, r;
    if (p < 0.02425) {
        q = std::sqrt(-2.0 * std::log(p));
        return (((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
               ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1.0);
    } else if (p <= 0.97575) {
        q = p - 0.5;
        r = q * q;
        return (((((a[0]*r+a[1])*r+a[2])*r+a[3])*r+a[4])*r+a[5])*q /
               (((((b[0]*r+b[1])*r+b[2])*r+b[3])*r+b[4])*r+1.0);
    } else {
        q = std::sqrt(-2.0 * std::log(1.0 - p));
        return -(((((c[0]*q+c[1])*q+c[2])*q+c[3])*q+c[4])*q+c[5]) /
                ((((d[0]*q+d[1])*q+d[2])*q+d[3])*q+1.0);
    }
}

double InventoryPolicyEngine::calculateSafetyStock(double meanDailyDemand, double stdDevDailyDemand,
                                                   double leadTimeDays, double targetServiceLevel) {
    if (leadTimeDays <= 0.0 || targetServiceLevel <= 0.0) return 0.0;
    double z = normalInverseCDF(targetServiceLevel);
    // sigma_L = sqrt(L * sigma_D^2)
    double stdDevLeadTime = stdDevDailyDemand * std::sqrt(leadTimeDays);
    return std::max(0.0, z * stdDevLeadTime);
}

PolicyConfig InventoryPolicyEngine::calibratePolicy(double meanDailyDemand, double stdDevDailyDemand,
                                                    double leadTimeDays, double holdingCostPerUnitDay,
                                                    double fixedOrderCost, double unitPurchaseCost,
                                                    double targetFillRate) {
    // 1. Calculate safety stock for target service level
    double ss = calculateSafetyStock(meanDailyDemand, stdDevDailyDemand, leadTimeDays, targetFillRate);

    // 2. Expected lead time demand
    double leadTimeDemand = meanDailyDemand * leadTimeDays;

    // 3. Reorder point s = LeadTimeDemand + SafetyStock
    int s = static_cast<int>(std::ceil(leadTimeDemand + ss));

    // 4. Economic Order Quantity (EOQ): sqrt(2 * D * S / H)
    // Annual demand D_annual = meanDailyDemand * 365
    // Annual holding cost H_annual = holdingCostPerUnitDay * 365
    double annualDemand = meanDailyDemand * 365.0;
    double annualHolding = holdingCostPerUnitDay * 365.0;
    if (annualHolding <= 0.0001) annualHolding = 0.0001;

    double eoq = std::sqrt((2.0 * annualDemand * fixedOrderCost) / annualHolding);
    int batchQ = static_cast<int>(std::max(5.0, std::round(eoq)));

    // 5. Order up to level S = s + Q
    int S = s + batchQ;

    return PolicyConfig(s, S, targetFillRate, ss);
}

} // namespace optichain
