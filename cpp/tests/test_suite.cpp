#include "network.hpp"
#include "inventory_policy.hpp"
#include "discrete_event_sim.hpp"
#include "spare_parts_optimizer.hpp"
#include "warranty_model.hpp"

#include <iostream>
#include <cassert>
#include <cmath>

using namespace optichain;

void testNetworkDAG() {
    SupplyNetwork net;
    int cdc = net.addNode("CDC", "IL", EchelonTier::CENTRAL_DC, -1, 5.0, 0.2, 100.0, 300.0, 200.0);
    int rdc = net.addNode("RDC", "PA", EchelonTier::REGIONAL_DC, cdc, 2.0, 0.3, 120.0, 350.0, 100.0);
    int hub = net.addNode("Hub", "NY", EchelonTier::LOCAL_SERVICE_HUB, rdc, 1.0, 0.4, 150.0, 380.0, 50.0);

    assert(net.size() == 3);
    assert(net.validateNetwork());

    auto downstream = net.getTopologicalOrderDownstream();
    assert(downstream.size() == 3);
    assert(downstream[0] == cdc);
    assert(downstream[1] == rdc);
    assert(downstream[2] == hub);

    auto upstream = net.getTopologicalOrderUpstream();
    assert(upstream[0] == hub);
    assert(upstream[1] == rdc);
    assert(upstream[2] == cdc);

    std::cout << "[PASS] testNetworkDAG\n";
}

void testSafetyStockAndPolicy() {
    double z95 = InventoryPolicyEngine::normalInverseCDF(0.95);
    assert(std::abs(z95 - 1.645) < 0.05);

    double ss = InventoryPolicyEngine::calculateSafetyStock(10.0, 2.0, 4.0, 0.95);
    assert(ss > 0.0);

    PolicyConfig pol = InventoryPolicyEngine::calibratePolicy(10.0, 2.0, 4.0, 0.2, 50.0, 100.0, 0.95);
    assert(pol.reorderPoint > 40);
    assert(pol.orderUpToLevel > pol.reorderPoint);

    std::cout << "[PASS] testSafetyStockAndPolicy\n";
}

void testDiscreteEventSim() {
    SupplyNetwork net;
    int cdc = net.addNode("CDC", "IL", EchelonTier::CENTRAL_DC, -1, 2.0, 0.1, 50.0, 100.0, 50.0);
    int hub = net.addNode("Hub", "NY", EchelonTier::LOCAL_SERVICE_HUB, cdc, 1.0, 0.2, 100.0, 120.0, 20.0);

    DiscreteEventSimulator sim(net, 42);
    sim.setCustomerDemandParams(hub, 5.0, 1.0);
    PolicyConfig pol(15, 40, 0.95, 5.0);
    sim.setNodePolicy(cdc, pol);
    sim.setNodePolicy(hub, pol);

    SimulationResult res = sim.runSimulation(30);
    assert(res.simulationDays == 30);
    assert(res.averageNetworkFillRate > 0.5);
    assert(res.totalNetworkCost > 0.0);

    std::cout << "[PASS] testDiscreteEventSim\n";
}

void testSparePartsOptimizer() {
    SparePartsOptimizer opt;
    opt.addItem(SparePartItem(1, "ITEM-A", "Critical", 1000.0, 2.0, 500.0, 10, 5));
    opt.addItem(SparePartItem(2, "ITEM-B", "Standard", 200.0, 5.0, 150.0, 7, 10));

    OptimizerResult res = opt.optimizeMarginalAllocation(5000.0);
    assert(res.spentBudget <= 5000.0);
    assert(res.allocations.size() == 2);
    assert(res.expectedSystemAvailability > 0.70);

    OptimizerResult dpRes = opt.optimizeDynamicProgramming(5000.0, 50);
    assert(dpRes.spentBudget <= 5000.0);

    std::cout << "[PASS] testSparePartsOptimizer\n";
}

void testWarrantyForecasting() {
    WarrantyProfile prof;
    prof.productFamily = "TestTurbine";
    prof.installedBase = 1000;
    prof.warrantyPeriodMonths = 12;
    prof.weibullShapeBeta = 1.5;
    prof.weibullScaleEta = 48.0;
    prof.averageClaimCost = 500.0;
    prof.preventativeInspectionCost = 100.0;

    WarrantyAnalysisReport rep = WarrantyForecastingEngine::forecastWarrantyLiabilities(prof);
    assert(rep.monthlyProjections.size() == 12);
    assert(rep.totalExpectedClaims > 0);
    assert(rep.totalWarrantyReserveUSD > 0);
    assert(rep.proactiveSavingsUSD >= 0);

    std::cout << "[PASS] testWarrantyForecasting\n";
}

int main() {
    std::cout << "--- Running OptiChain C++ Unit Test Suite ---\n";
    testNetworkDAG();
    testSafetyStockAndPolicy();
    testDiscreteEventSim();
    testSparePartsOptimizer();
    testWarrantyForecasting();
    std::cout << "All unit tests passed successfully!\n";
    return 0;
}
