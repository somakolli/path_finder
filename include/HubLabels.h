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

namespace algEng {
class HubLabels : public PathFinderBase{
private:
    std::vector<std::vector<CostNode>> hubLabels;
    std::vector<std::vector<CostNode>> backHubLabels;
    std::vector<CHNode> sortedNodes;
    CHGraph& graph;
    void setLabel(NodeId nodeId, EdgeDirection direction);
    void processRange(std::pair<uint32_t, uint32_t> range, EdgeDirection direction);
    void processRangeAsync(std::pair<uint32_t, uint32_t> range, EdgeDirection direction, uint32_t maxNumberOfThreads);
public:
    std::vector<CostNode>& getLabels(NodeId nodeId, EdgeDirection direction);
    explicit HubLabels(CHGraph &graph);
    std::optional<Distance> getShortestDistance(NodeId source, NodeId target);
    [[nodiscard]]static Distance getDistance(std::vector<CostNode> &forwardLabels, std::vector<CostNode> &backwardLabels, NodeId& nodeId);
    static void sortLabel(std::vector<CostNode> &label);
    void constructAllLabels(int maxThreads, const std::vector<std::pair<uint32_t, uint32_t >>& sameLevelRanges, const EdgeDirection &direction);
    void selfPrune(NodeId labelId, EdgeDirection direction);
    void constructAllLabelsAsync(int maxThreads, const std::vector<std::pair<uint32_t, uint32_t >>& sameLevelRanges, const EdgeDirection &direction);
    void setLabelAsync(NodeId& begin, const NodeId& end, std::mutex& mutex, EdgeDirection direction);
    std::vector<LatLng> getShortestPath(NodeId source, NodeId target);
};
}
#endif //ALG_ENG_PROJECT_HUBLABELS_H
