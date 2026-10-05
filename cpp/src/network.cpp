#include "network.hpp"
#include <queue>
#include <algorithm>

namespace optichain {

int SupplyNetwork::addNode(const std::string& name, const std::string& location, EchelonTier tier,
                           int parentId, double leadTimeDays, double holdingCost, double backorderPenalty,
                           double purchaseCost, double fixedOrderCost) {
    int id = static_cast<int>(nodes_.size());
    nodes_.emplace_back(id, name, location, tier, parentId, leadTimeDays, holdingCost,
                        backorderPenalty, purchaseCost, fixedOrderCost);
    idToIndex_[id] = nodes_.size() - 1;

    if (parentId >= 0 && parentId < static_cast<int>(nodes_.size()) - 1) {
        nodes_[parentId].childrenIds.push_back(id);
    }
    return id;
}

const Node& SupplyNetwork::getNode(int id) const {
    auto it = idToIndex_.find(id);
    if (it == idToIndex_.end()) {
        throw std::out_of_range("Node ID not found in network: " + std::to_string(id));
    }
    return nodes_[it->second];
}

Node& SupplyNetwork::getNode(int id) {
    auto it = idToIndex_.find(id);
    if (it == idToIndex_.end()) {
        throw std::out_of_range("Node ID not found in network: " + std::to_string(id));
    }
    return nodes_[it->second];
}

const std::vector<Node>& SupplyNetwork::getAllNodes() const {
    return nodes_;
}

std::vector<int> SupplyNetwork::getTopologicalOrderDownstream() const {
    // Kahn's algorithm or BFS from root (tier 0 down to tier 2)
    std::vector<int> result;
    std::queue<int> q;

    for (const auto& node : nodes_) {
        if (node.parentId == -1) {
            q.push(node.id);
        }
    }

    while (!q.empty()) {
        int currId = q.front();
        q.pop();
        result.push_back(currId);

        const auto& currNode = getNode(currId);
        for (int childId : currNode.childrenIds) {
            q.push(childId);
        }
    }

    return result;
}

std::vector<int> SupplyNetwork::getTopologicalOrderUpstream() const {
    std::vector<int> order = getTopologicalOrderDownstream();
    std::reverse(order.begin(), order.end());
    return order;
}

bool SupplyNetwork::validateNetwork() const {
    if (nodes_.empty()) return false;
    
    // Check root node
    int rootCount = 0;
    for (const auto& node : nodes_) {
        if (node.parentId == -1) {
            rootCount++;
        } else {
            if (idToIndex_.find(node.parentId) == idToIndex_.end()) {
                return false; // Invalid parent
            }
        }
    }
    return rootCount >= 1;
}

} // namespace optichain
