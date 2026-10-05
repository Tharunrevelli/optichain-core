#include "warranty_model.hpp"
#include <cmath>
#include <algorithm>

namespace optichain {

double WarrantyForecastingEngine::weibullCDF(double t, double beta, double eta) {
    if (t <= 0.0 || eta <= 0.0) return 0.0;
    return 1.0 - std::exp(-std::pow(t / eta, beta));
}

WarrantyAnalysisReport WarrantyForecastingEngine::forecastWarrantyLiabilities(const WarrantyProfile& profile) {
    WarrantyAnalysisReport report;
    report.productFamily = profile.productFamily;
    report.totalInstalledBase = profile.installedBase;
    report.warrantyPeriodMonths = profile.warrantyPeriodMonths;
    report.totalExpectedClaims = 0.0;
    report.totalWarrantyReserveUSD = 0.0;

    double cumulativeReserve = 0.0;
    double prevCDF = 0.0;

    for (int m = 1; m <= profile.warrantyPeriodMonths; ++m) {
        double currCDF = weibullCDF(static_cast<double>(m), profile.weibullShapeBeta, profile.weibullScaleEta);
        double monthlyP = std::max(0.0, currCDF - prevCDF);
        prevCDF = currCDF;

        int claims = static_cast<int>(std::round(profile.installedBase * monthlyP));
        double claimCost = claims * profile.averageClaimCost;
        cumulativeReserve += claimCost;

        MonthlyWarrantyForecast forecast;
        forecast.month = m;
        forecast.failureProbability = monthlyP;
        forecast.expectedClaims = claims;
        forecast.totalClaimCost = claimCost;
        forecast.cumulativeReserveRequired = cumulativeReserve;

        report.monthlyProjections.push_back(forecast);
        report.totalExpectedClaims += claims;
    }

    report.totalWarrantyReserveUSD = cumulativeReserve;

    // Proactive refurbishment calculation:
    // If telemetry/inspection intercepts 35% of failures early, cost drops from averageClaimCost to preventativeInspectionCost
    double interceptedClaims = report.totalExpectedClaims * 0.35;
    double unoptimizedCost = interceptedClaims * profile.averageClaimCost;
    double optimizedCost = interceptedClaims * profile.preventativeInspectionCost;
    report.proactiveSavingsUSD = std::max(0.0, unoptimizedCost - optimizedCost);

    return report;
}

} // namespace optichain
