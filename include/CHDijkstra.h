//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHDIJKSTRA_H
#define ALG_ENG_PROJECT_CHDIJKSTRA_H

#include <set>
#include "CHGraph.h"
#include "PathFinderBase.h"

namespace pathFinder {
class CHDijkstra : public PathFinderBase{
public:
    CHDijkstra(CHGraph &graph);
    std::optional<Distance> getShortestDistance(NodeId source, NodeId target);
    std::vector<CostNode> shortestDistance(NodeId source, EdgeDirection direction);
    uint32_t hopCountUntilLevel(NodeId source, Level level, EdgeDirection direction);
    std::vector<LatLng> getShortestPath(NodeId source, NodeId target);
private:
    std::vector<Distance> cost;
    std::vector<NodeId> visited;
    CHGraph& graph;
};
}

#endif //ALG_ENG_PROJECT_CHDIJKSTRA_H
