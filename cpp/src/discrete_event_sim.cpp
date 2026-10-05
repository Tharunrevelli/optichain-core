#include "discrete_event_sim.hpp"
#include <chrono>
#include <algorithm>
#include <iostream>

namespace optichain {

DiscreteEventSimulator::DiscreteEventSimulator(const SupplyNetwork& network, unsigned int randomSeed)
    : network_(network), seed_(randomSeed), rng_(randomSeed) {
    // Initialize node states
    for (const auto& node : network_.getAllNodes()) {
        NodeState state;
        state.onHand = 50; // Initial default buffer
        state.onOrder = 0;
        state.backordered = 0;
        state.policy = PolicyConfig(20, 60, 0.95, 10.0);
        nodeStates_[node.id] = state;
    }
}

void DiscreteEventSimulator::setNodePolicy(int nodeId, const PolicyConfig& policy) {
    nodeStates_[nodeId].policy = policy;
    // Set initial on-hand close to order-up-to level
    nodeStates_[nodeId].onHand = policy.orderUpToLevel;
}

void DiscreteEventSimulator::setCustomerDemandParams(int nodeId, double meanDailyDemand, double stdDevDailyDemand) {
    demandParams_[nodeId] = {meanDailyDemand, stdDevDailyDemand};
}

void DiscreteEventSimulator::scheduleNextDemand(int nodeId, double currentTime) {
    auto it = demandParams_.find(nodeId);
    if (it == demandParams_.end()) return;

    double mean = it->second.first;
    double stdDev = it->second.second;
    if (mean <= 0.0) return;

    // Exponential inter-arrival times: lambda = mean per day, average inter-arrival = 1 / mean
    // Or schedule 1 aggregated demand per day with Poisson/Normal quantity
    std::exponential_distribution<double> expDist(mean);
    double interArrival = std::max(0.05, expDist(rng_));

    std::normal_distribution<double> qtyDist(1.0, 0.3);
    int qty = std::max(1, static_cast<int>(std::round(qtyDist(rng_))));

    SimulationEvent ev;
    ev.timestamp = currentTime + interArrival;
    ev.type = EventType::CUSTOMER_DEMAND;
    ev.nodeId = nodeId;
    ev.quantity = qty;
    ev.orderId = ++nextOrderId_;
    eventQueue_.push(ev);
}

SimulationResult DiscreteEventSimulator::runSimulation(int days) {
    auto startTime = std::chrono::high_resolution_clock::now();

    // Clear queue
    while (!eventQueue_.empty()) eventQueue_.pop();

    // Schedule daily accounting events for all days
    for (int d = 1; d <= days; ++d) {
        SimulationEvent ev;
        ev.timestamp = static_cast<double>(d);
        ev.type = EventType::DAILY_ACCOUNTING;
        ev.nodeId = -1; // global
        ev.quantity = 0;
        ev.orderId = 0;
        eventQueue_.push(ev);
    }

    // Schedule initial customer demands for leaf nodes (Local Service Hubs)
    for (const auto& kv : demandParams_) {
        scheduleNextDemand(kv.first, 0.0);
    }

    // Process event loop
    while (!eventQueue_.empty()) {
        SimulationEvent ev = eventQueue_.top();
        eventQueue_.pop();

        if (ev.timestamp > static_cast<double>(days)) {
            break;
        }

        switch (ev.type) {
            case EventType::CUSTOMER_DEMAND:
                handleDemand(ev);
                break;
            case EventType::SHIPMENT_ARRIVAL:
                handleShipmentArrival(ev);
                break;
            case EventType::DAILY_ACCOUNTING:
                handleDailyAccounting(ev);
                break;
            default:
                break;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double execMicroseconds = std::chrono::duration<double, std::micro>(endTime - startTime).count();

    // Collect results
    SimulationResult result;
    result.simulationDays = days;
    result.runTimeMicroseconds = execMicroseconds;
    result.totalNetworkCost = 0.0;

    double sumFillRates = 0.0;
    int evaluatedHubs = 0;

    for (auto& kv : nodeStates_) {
        int nodeId = kv.first;
        NodeState& state = kv.second;
        
        // Finalize total cost
        state.metrics.totalCost = state.metrics.totalHoldingCost +
                                  state.metrics.totalOrderingCost +
                                  state.metrics.totalStockoutPenalty;

        result.nodeMetrics[nodeId] = state.metrics;
        result.nodeDailyStock[nodeId] = state.dailyInventoryHistory;
        result.totalNetworkCost += state.metrics.totalCost;

        if (demandParams_.find(nodeId) != demandParams_.end()) {
            sumFillRates += state.metrics.achievedFillRate();
            evaluatedHubs++;
        }
    }

    result.averageNetworkFillRate = evaluatedHubs > 0 ? (sumFillRates / evaluatedHubs) : 1.0;
    return result;
}

void DiscreteEventSimulator::handleDemand(const SimulationEvent& ev) {
    int nodeId = ev.nodeId;
    auto& state = nodeStates_[nodeId];
    int requested = ev.quantity;

    state.metrics.totalDemandUnits += requested;

    if (state.onHand >= requested) {
        state.onHand -= requested;
        state.metrics.fulfilledUnits += requested;
    } else {
        int fulfilled = std::max(0, state.onHand);
        int unfulfilled = requested - fulfilled;
        state.onHand = 0;
        state.backordered += unfulfilled;
        state.metrics.fulfilledUnits += fulfilled;
        state.metrics.backorderedUnits += unfulfilled;
        state.metrics.stockoutEvents++;

        const auto& node = network_.getNode(nodeId);
        state.metrics.totalStockoutPenalty += unfulfilled * node.backorderPenaltyPerUnit;
    }

    // Check if replenishment needed
    triggerReplenishmentCheck(nodeId, ev.timestamp);

    // Schedule next customer arrival
    scheduleNextDemand(nodeId, ev.timestamp);
}

void DiscreteEventSimulator::triggerReplenishmentCheck(int nodeId, double currentTime) {
    auto& state = nodeStates_[nodeId];
    int inventoryPosition = state.onHand + state.onOrder - state.backordered;

    if (inventoryPosition <= state.policy.reorderPoint) {
        int orderQty = state.policy.orderUpToLevel - inventoryPosition;
        if (orderQty <= 0) return;

        state.onOrder += orderQty;
        state.metrics.replenishmentOrdersPlaced++;
        const auto& node = network_.getNode(nodeId);
        state.metrics.totalOrderingCost += node.fixedOrderCost;

        // Determine arrival time based on parent echelon
        double leadTime = node.leadTimeDays;
        
        if (node.parentId != -1) {
            auto& parentState = nodeStates_[node.parentId];
            // If parent has inventory, fulfill and schedule arrival at child
            if (parentState.onHand >= orderQty) {
                parentState.onHand -= orderQty;
            } else {
                // Parent stockout causes delay (multi-echelon delay propagation)
                leadTime += 3.0; // cross-echelon delay penalty
                parentState.backordered += (orderQty - std::max(0, parentState.onHand));
                parentState.onHand = 0;
            }
            triggerReplenishmentCheck(node.parentId, currentTime);
        }

        SimulationEvent arrivalEv;
        arrivalEv.timestamp = currentTime + leadTime;
        arrivalEv.type = EventType::SHIPMENT_ARRIVAL;
        arrivalEv.nodeId = nodeId;
        arrivalEv.quantity = orderQty;
        arrivalEv.orderId = ++nextOrderId_;
        eventQueue_.push(arrivalEv);
    }
}

void DiscreteEventSimulator::handleShipmentArrival(const SimulationEvent& ev) {
    int nodeId = ev.nodeId;
    auto& state = nodeStates_[nodeId];
    int arriving = ev.quantity;

    state.onOrder = std::max(0, state.onOrder - arriving);

    // Fulfill backorders first
    if (state.backordered > 0) {
        int fulfillBackorder = std::min(state.backordered, arriving);
        state.backordered -= fulfillBackorder;
        arriving -= fulfillBackorder;
    }

    state.onHand += arriving;
}

void DiscreteEventSimulator::handleDailyAccounting(const SimulationEvent& ev) {
    for (auto& kv : nodeStates_) {
        int nodeId = kv.first;
        NodeState& state = kv.second;
        const auto& node = network_.getNode(nodeId);

        // Daily holding cost accrued on positive on-hand inventory
        if (state.onHand > 0) {
            state.metrics.totalHoldingCost += state.onHand * node.holdingCostPerUnitPerDay;
        }

        // Daily backorder penalty
        if (state.backordered > 0) {
            state.metrics.totalStockoutPenalty += state.backordered * (node.backorderPenaltyPerUnit * 0.1);
        }

        // Record history snapshot
        state.dailyInventoryHistory.push_back(state.onHand);
    }
}

} // namespace optichain
