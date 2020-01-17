//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHDIJKSTRA_H
#define ALG_ENG_PROJECT_CHDIJKSTRA_H

#include <set>
#include "CHGraph.h"
#include "PathFinderBase.h"
#include "HubLabels.h"

namespace pathFinder {
    template<typename Graph>
class CHDijkstra : public PathFinderBase{
public:
    explicit CHDijkstra(Graph &graph);
    std::optional<Distance> getShortestDistance(NodeId source, NodeId target) override;
    std::vector<CostNode> shortestDistance(NodeId source, EdgeDirection direction);
    std::vector<LatLng> getShortestPath(NodeId source, NodeId target) override;
private:
    std::vector<Distance> cost;
    std::vector<NodeId> visited;
    Graph& graph;
};
}

#endif //ALG_ENG_PROJECT_CHDIJKSTRA_H
