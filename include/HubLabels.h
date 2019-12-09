//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_HUBLABELS_H
#define ALG_ENG_PROJECT_HUBLABELS_H

#include <vector>
#include <set>
#include <map>
#include "Graph.h"
#include "CHGraph.h"
#include "thread"
#include <boost/container/small_vector.hpp>
#include "queue"
#include "mutex"
#include "PathFinderBase.h"

namespace pathFinder {

typedef std::vector<CostNode> costNodeVec_t;

class HubLabels : public PathFinderBase {

private:
    static const size_t labelsUntilLevel = 20;
    std::vector<costNodeVec_t> hubLabels;
    std::vector<costNodeVec_t> backHubLabels;
    std::vector<CHNode> sortedNodes;
    std::vector<Distance> cost;
    std::vector<NodeId> visited;
    CHGraph& graph;
    void setLabel(NodeId nodeId, EdgeDirection direction);
    void processRange(std::pair<uint32_t, uint32_t> range, EdgeDirection direction);
    void constructAllLabels(const std::vector<std::pair<uint32_t, uint32_t >>& sameLevelRanges, int maxLevel, int minLevel);
    void selfPrune(NodeId labelId, EdgeDirection direction);
    costNodeVec_t calcLabel(NodeId source, EdgeDirection direction);
    void mergeLabels(std::vector<CostNode>& label1, const std::vector<CostNode>& label2, Distance edgeDistance);
public:
    pathFinder::costNodeVec_t& getLabels(NodeId nodeId, EdgeDirection direction);
    explicit HubLabels(CHGraph &graph);
    std::optional<Distance> getShortestDistance(NodeId source, NodeId target) override;
    static std::optional<Distance> getShortestDistance(costNodeVec_t &forwardLabels, costNodeVec_t &backwardLabels, NodeId& nodeId);
    static void sortLabel(costNodeVec_t &label);
    std::vector<LatLng> getShortestPath(NodeId source, NodeId target) override;
};
}
#endif //ALG_ENG_PROJECT_HUBLABELS_H
