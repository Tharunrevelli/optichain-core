#ifndef OPTICHAIN_NETWORK_HPP
#define OPTICHAIN_NETWORK_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace optichain {

enum class EchelonTier {
    CENTRAL_DC = 0,    // Tier 0: Central Distribution Center
    REGIONAL_DC = 1,   // Tier 1: Regional Distribution Center
    LOCAL_SERVICE_HUB = 2 // Tier 2: Field Service Center / Dealership
};

inline std::string tierToString(EchelonTier tier) {
    switch (tier) {
        case EchelonTier::CENTRAL_DC: return "Central DC (Tier 0)";
        case EchelonTier::REGIONAL_DC: return "Regional DC (Tier 1)";
        case EchelonTier::LOCAL_SERVICE_HUB: return "Local Hub (Tier 2)";
        default: return "Unknown";
    }
}

struct Node {
    int id;
    std::string name;
    std::string location;
    EchelonTier tier;
    int parentId; // -1 if root (Central DC)
    double leadTimeDays;
    double holdingCostPerUnitPerDay;
    double backorderPenaltyPerUnit;
    double unitPurchaseCost;
    double fixedOrderCost;
    std::vector<int> childrenIds;

    Node(int id, std::string name, std::string location, EchelonTier tier,
         int parentId, double leadTimeDays, double holdingCost, double backorderPenalty,
         double purchaseCost, double fixedOrderCost)
        : id(id), name(std::move(name)), location(std::move(location)), tier(tier),
          parentId(parentId), leadTimeDays(leadTimeDays),
          holdingCostPerUnitPerDay(holdingCost),
          backorderPenaltyPerUnit(backorderPenalty),
          unitPurchaseCost(purchaseCost),
          fixedOrderCost(fixedOrderCost) {}
};

class SupplyNetwork {
public:
    SupplyNetwork() = default;

    int addNode(const std::string& name, const std::string& location, EchelonTier tier,
                int parentId, double leadTimeDays, double holdingCost, double backorderPenalty,
                double purchaseCost, double fixedOrderCost);

    const Node& getNode(int id) const;
    Node& getNode(int id);
    const std::vector<Node>& getAllNodes() const;
    size_t size() const { return nodes_.size(); }

    // Topological order from Central DC (root) down to Local Hubs (leaves)
    std::vector<int> getTopologicalOrderDownstream() const;
    // Topological order from Local Hubs (leaves) up to Central DC (root)
    std::vector<int> getTopologicalOrderUpstream() const;

    // Validate network structure: ensure tree/DAG acyclic, valid parent references
    bool validateNetwork() const;

private:
    std::vector<Node> nodes_;
    std::unordered_map<int, size_t> idToIndex_;
};

} // namespace optichain

#endif // OPTICHAIN_NETWORK_HPP
