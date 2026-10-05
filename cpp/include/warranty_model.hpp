#ifndef OPTICHAIN_WARRANTY_MODEL_HPP
#define OPTICHAIN_WARRANTY_MODEL_HPP

#include <string>
#include <vector>

namespace optichain {

struct WarrantyProfile {
    std::string productFamily;
    int installedBase;
    int warrantyPeriodMonths;
    double weibullShapeBeta;   // beta > 1 implies infant wear-out or wear-out phase
    double weibullScaleEta;    // characteristic life in months
    double averageClaimCost;   // cost per warranty claim incident
    double preventativeInspectionCost;
};

struct MonthlyWarrantyForecast {
    int month;
    double failureProbability;
    int expectedClaims;
    double totalClaimCost;
    double cumulativeReserveRequired;
};

struct WarrantyAnalysisReport {
    std::string productFamily;
    int totalInstalledBase;
    int warrantyPeriodMonths;
    double totalExpectedClaims;
    double totalWarrantyReserveUSD;
    double proactiveSavingsUSD; // savings if predictive inspection intercepts 35% of catastrophic claims
    std::vector<MonthlyWarrantyForecast> monthlyProjections;
};

class WarrantyForecastingEngine {
public:
    static WarrantyAnalysisReport forecastWarrantyLiabilities(const WarrantyProfile& profile);

private:
    static double weibullCDF(double t, double beta, double eta);
};

} // namespace optichain

#endif // OPTICHAIN_WARRANTY_MODEL_HPP
