#include "network.hpp"
#include "inventory_policy.hpp"
#include "discrete_event_sim.hpp"
#include "spare_parts_optimizer.hpp"
#include "warranty_model.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>

using namespace optichain;

void printBanner() {
    std::cout << "======================================================================\n";
    std::cout << "  OptiChain-Core: High-Performance Multi-Echelon Supply Chain Engine  \n";
    std::cout << "  C++ Discrete-Event Simulation & Constrained Spare Parts Optimizer   \n";
    std::cout << "======================================================================\n\n";
}

int main(int argc, char* argv[]) {
    printBanner();

    // 1. Build Multi-Echelon Supply Chain Network
    std::cout << "[Step 1/4] Initializing Multi-Echelon Supply Network Topology...\n";
    SupplyNetwork network;

    // Root: Central DC (Chicago Hub)
    int cdcId = network.addNode("Central Distribution Center", "Chicago, IL",
                                EchelonTier::CENTRAL_DC, -1,
                                7.0,   // Supplier Lead Time: 7 days
                                0.25,  // Daily holding cost: $0.25/unit
                                120.0, // Backorder penalty: $120/unit
                                450.0, // Purchase cost: $450
                                500.0  // Fixed order cost: $500
    );

    // Tier 1: Regional Distribution Centers
    int rdcEast = network.addNode("East Coast RDC", "Allentown, PA",
                                  EchelonTier::REGIONAL_DC, cdcId,
                                  2.5, 0.35, 140.0, 480.0, 150.0);
    int rdcWest = network.addNode("West Coast RDC", "Reno, NV",
                                  EchelonTier::REGIONAL_DC, cdcId,
                                  3.0, 0.38, 140.0, 480.0, 160.0);

    // Tier 2: Local Service Hubs (Dealerships / Field Service Centers)
    int hubNYC = network.addNode("New York Metro Service Hub", "Queens, NY",
                                 EchelonTier::LOCAL_SERVICE_HUB, rdcEast,
                                 1.0, 0.55, 250.0, 520.0, 50.0);
    int hubBOS = network.addNode("Boston Industrial Hub", "Waltham, MA",
                                 EchelonTier::LOCAL_SERVICE_HUB, rdcEast,
                                 1.2, 0.50, 240.0, 520.0, 45.0);
    int hubSF  = network.addNode("SF Bay Area Hub", "Oakland, CA",
                                 EchelonTier::LOCAL_SERVICE_HUB, rdcWest,
                                 1.0, 0.60, 280.0, 530.0, 55.0);
    int hubLA  = network.addNode("LA Basin Hub", "Long Beach, CA",
                                 EchelonTier::LOCAL_SERVICE_HUB, rdcWest,
                                 1.1, 0.58, 270.0, 530.0, 50.0);

    std::cout << "  Network validated: " << (network.validateNetwork() ? "PASSED (DAG Acyclic)" : "FAILED") << "\n";
    std::cout << "  Total Nodes in Multi-Echelon Graph: " << network.size() << "\n\n";

    // 2. Calibrate (s, S) Continuous Review Policies
    std::cout << "[Step 2/4] Calibrating Stochastic Inventory Policies across Nodes...\n";
    DiscreteEventSimulator sim(network, 1337);

    // Demand parameters at service hubs (mean, stdDev per day)
    sim.setCustomerDemandParams(hubNYC, 4.5, 1.8);
    sim.setCustomerDemandParams(hubBOS, 3.2, 1.2);
    sim.setCustomerDemandParams(hubSF,  4.0, 1.5);
    sim.setCustomerDemandParams(hubLA,  5.1, 2.0);

    // Calibrate and assign policies
    for (const auto& node : network.getAllNodes()) {
        double meanD = (node.tier == EchelonTier::LOCAL_SERVICE_HUB) ? 4.0 : 8.0;
        double stdD = (node.tier == EchelonTier::LOCAL_SERVICE_HUB) ? 1.5 : 2.5;
        PolicyConfig pol = InventoryPolicyEngine::calibratePolicy(
            meanD, stdD, node.leadTimeDays, node.holdingCostPerUnitPerDay,
            node.fixedOrderCost, node.unitPurchaseCost, 0.98
        );
        sim.setNodePolicy(node.id, pol);
        std::cout << "  Node [" << node.id << "] " << std::setw(28) << std::left << node.name
                  << " | s (ROP): " << std::setw(3) << pol.reorderPoint
                  << " | S (Cap): " << std::setw(3) << pol.orderUpToLevel
                  << " | Safety Stock: " << std::fixed << std::setprecision(1) << pol.safetyStock << "\n";
    }

    // Run 365-Day Discrete Event Simulation
    int simDays = 365;
    std::cout << "\n  Executing 365-Day Discrete-Event Simulation using Min-Heap priority queue...\n";
    SimulationResult simRes = sim.runSimulation(simDays);

    std::cout << "  Simulation finished in: " << std::fixed << std::setprecision(2)
              << simRes.runTimeMicroseconds << " microseconds ("
              << (simRes.runTimeMicroseconds / 1000.0) << " ms)\n";
    std::cout << "  Average Network Fill Rate: " << std::fixed << std::setprecision(2)
              << (simRes.averageNetworkFillRate * 100.0) << "%\n";
    std::cout << "  Total Annual Operating Cost: $" << std::fixed << std::setprecision(2)
              << simRes.totalNetworkCost << "\n\n";

    // 3. Spare Parts Multi-Item Optimization
    std::cout << "[Step 3/4] Optimizing Spare Parts Capital Allocation (METRIC vs DP Knapsack)...\n";
    SparePartsOptimizer optimizer;
    optimizer.addItem(SparePartItem(1, "SKU-TURB-01", "Turbine Blades",     3200.0, 1.8, 850.0, 21, 6));
    optimizer.addItem(SparePartItem(2, "SKU-HYDR-04", "Hydraulic Actuator", 1850.0, 3.4, 450.0, 14, 8));
    optimizer.addItem(SparePartItem(3, "SKU-CTRL-09", "ECU Microcontroller",  620.0, 6.2, 300.0,  7, 15));
    optimizer.addItem(SparePartItem(4, "SKU-VALV-12", "Cryogenic Solenoid",   940.0, 4.1, 380.0, 10, 10));
    optimizer.addItem(SparePartItem(5, "SKU-BRNG-88", "Ceramic Roller Bearing", 480.0, 8.5, 220.0,  5, 20));

    double workingCapitalBudget = 75000.0; // $75,000 budget
    OptimizerResult optRes = optimizer.optimizeMarginalAllocation(workingCapitalBudget);

    std::cout << "  Budget: $" << workingCapitalBudget
              << " | Invested: $" << optRes.spentBudget
              << " | Expected Availability: " << std::setprecision(2) << (optRes.expectedSystemAvailability * 100.0) << "%\n";
    std::cout << "  Allocated SKUs:\n";
    for (const auto& a : optRes.allocations) {
        std::cout << "    Part " << std::setw(12) << a.sku
                  << " | Units: " << std::setw(2) << a.allocatedUnits
                  << " | Capital: $" << std::setw(7) << a.capitalInvestment
                  << " | Exp Backorders: " << std::setprecision(4) << a.expectedBackorders << "\n";
    }
    std::cout << "  Greedy Marginal PQ Runtime: " << std::setprecision(3) << optRes.executionTimeMs << " ms\n\n";

    // 4. Warranty Degradation & Reserve Forecasting
    std::cout << "[Step 4/4] Forecasting Warranty Liabilities & Preventative Reserve Needs...\n";
    WarrantyProfile prof;
    prof.productFamily = "Industrial Powertrain G4";
    prof.installedBase = 25000;
    prof.warrantyPeriodMonths = 36;
    prof.weibullShapeBeta = 1.45; // Wear-out phase
    prof.weibullScaleEta = 68.0;   // Characteristic life: 68 months
    prof.averageClaimCost = 1450.0;
    prof.preventativeInspectionCost = 280.0;

    WarrantyAnalysisReport wRep = WarrantyForecastingEngine::forecastWarrantyLiabilities(prof);

    std::cout << "  Product Family: " << wRep.productFamily << "\n";
    std::cout << "  Installed Base: " << wRep.totalInstalledBase << " units | Warranty Window: " << wRep.warrantyPeriodMonths << " months\n";
    std::cout << "  Projected Warranty Incidents: " << static_cast<int>(wRep.totalExpectedClaims) << " claims\n";
    std::cout << "  Standard Warranty Reserve Fund Required: $" << std::fixed << std::setprecision(2) << wRep.totalWarrantyReserveUSD << "\n";
    std::cout << "  Proactive Refurbishment Net Savings:     $" << std::fixed << std::setprecision(2) << wRep.proactiveSavingsUSD
              << " (" << std::setprecision(1) << (wRep.proactiveSavingsUSD / wRep.totalWarrantyReserveUSD * 100.0) << "% reduction)\n";

    std::cout << "\n======================================================================\n";
    std::cout << "  Engine Execution Completed Successfully.\n";
    std::cout << "======================================================================\n";

    return 0;
}
