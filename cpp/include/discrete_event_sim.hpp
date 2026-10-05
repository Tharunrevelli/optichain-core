#ifndef OPTICHAIN_DISCRETE_EVENT_SIM_HPP
#define OPTICHAIN_DISCRETE_EVENT_SIM_HPP

#include "network.hpp"
#include "inventory_policy.hpp"
#include <queue>
#include <vector>
#include <memory>
#include <random>
#include <functional>

namespace optichain {

enum class EventType {
    CUSTOMER_DEMAND,
    SHIPMENT_DEPARTURE,
    SHIPMENT_ARRIVAL,
    DAILY_ACCOUNTING
};

struct SimulationEvent {
    double timestamp; // in fractional days
    EventType type;
    int nodeId;
    int quantity;
    int orderId;

    bool operator>(const SimulationEvent& other) const {
        return timestamp > other.timestamp; // Min-heap ordered by earliest timestamp
    }
};

struct NodeState {
    int onHand;
    int onOrder; // in-flight shipments
    int backordered;
    InventoryMetrics metrics;
    PolicyConfig policy;
    std::vector<int> dailyInventoryHistory;
};

struct SimulationResult {
    int simulationDays;
    double runTimeMicroseconds;
    std::unordered_map<int, InventoryMetrics> nodeMetrics;
    std::unordered_map<int, std::vector<int>> nodeDailyStock;
    double totalNetworkCost;
    double averageNetworkFillRate;
};

class DiscreteEventSimulator {
public:
    DiscreteEventSimulator(const SupplyNetwork& network, unsigned int randomSeed = 42);

    void setNodePolicy(int nodeId, const PolicyConfig& policy);
    void setCustomerDemandParams(int nodeId, double meanDailyDemand, double stdDevDailyDemand);

    SimulationResult runSimulation(int days);

private:
    const SupplyNetwork& network_;
    unsigned int seed_;
    std::mt19937 rng_;
    
    std::priority_queue<SimulationEvent, std::vector<SimulationEvent>, std::greater<SimulationEvent>> eventQueue_;
    std::unordered_map<int, NodeState> nodeStates_;
    std::unordered_map<int, std::pair<double, double>> demandParams_; // nodeId -> (mean, stdDev)
    
    int nextOrderId_ = 1000;

    void handleDemand(const SimulationEvent& ev);
    void handleShipmentArrival(const SimulationEvent& ev);
    void handleDailyAccounting(const SimulationEvent& ev);
    void triggerReplenishmentCheck(int nodeId, double currentTime);
    void scheduleNextDemand(int nodeId, double currentTime);
};

} // namespace optichain

#endif // OPTICHAIN_DISCRETE_EVENT_SIM_HPP
