//
// Created by sokol on 03.10.19.
//
#include <queue>
#include <map>
#include "../include/CHDijkstra.h"
#include "algorithm"
#include "../include/HubLabels.h"
template<typename Graph>
std::vector<pathFinder::CostNode> pathFinder::CHDijkstra<Graph>::shortestDistance(pathFinder::NodeId source, pathFinder::EdgeDirection direction) {
    std::vector<CostNode> settledNodes;
    for(auto nodeId: visited)
        cost[nodeId] = MAX_DISTANCE;
    visited.clear();
    std::priority_queue<CostNode> q;
    q.emplace(source, 0);
    cost[source] = 0;
    visited.emplace_back(source);
    settledNodes.emplace_back(source, 0);
    while(!q.empty()) {
        auto costNode = q.top();
        q.pop();
        if(costNode.cost > cost[costNode.id])
            continue;
        settledNodes.push_back(costNode);
        for(const auto& edge: graph.edgesFor(costNode.id, direction)) {
            if(graph.getLevel(edge.source) > graph.getLevel(edge.target))
                continue;
            auto addedCost = costNode.cost + edge.distance;
            if(addedCost < cost[edge.target]) {
                visited.emplace_back(edge.target);
                cost[edge.target] = addedCost;
                q.emplace(edge.target, addedCost);
            }
        }
    }
    return settledNodes;
}
template <typename Graph>
pathFinder::CHDijkstra<Graph>::CHDijkstra(Graph &graph) : graph(graph) {
    cost.reserve(graph.numberOfNodes);
    while(cost.size() <= graph.numberOfNodes)
        cost.emplace_back(MAX_DISTANCE);
}
template<typename Graph>
std::optional<pathFinder::Distance> pathFinder::CHDijkstra<Graph>::getShortestDistance(pathFinder::NodeId source, pathFinder::NodeId target) {
    if(source >= graph.nodes().size() || target >= graph.nodes().size())
        return std::nullopt;
    auto forwardLabel = shortestDistance(source, EdgeDirection::FORWARD);
    auto backwardLabel = shortestDistance(target, EdgeDirection::BACKWARD);

    HubLabels<Graph>::sortLabel(forwardLabel);
    HubLabels<Graph>::sortLabel(backwardLabel);
    NodeId topNode;
    return pathFinder::HubLabels<Graph>::getShortestDistance(forwardLabel, backwardLabel, topNode);
}

template<typename Graph>
std::vector<pathFinder::LatLng> pathFinder::CHDijkstra<Graph>::getShortestPath(pathFinder::NodeId source, pathFinder::NodeId target) {
    return std::vector<LatLng>();
}
template class pathFinder::CHDijkstra<pathFinder::CHGraph<std::vector<pathFinder::Edge>, std::vector<pathFinder::CHNode>, std::vector<uint32_t>>>;
template class pathFinder::CHDijkstra<pathFinder::DiskGraph> ;