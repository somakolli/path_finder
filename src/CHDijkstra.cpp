//
// Created by sokol on 03.10.19.
//
#include <queue>
#include <map>
#include "../include/CHDijkstra.h"
#include "algorithm"
#include "../include/HubLabels.h"

std::vector<pathFinder::CostNode> pathFinder::CHDijkstra::shortestDistance(pathFinder::NodeId source, pathFinder::EdgeDirection direction) {
    std::vector<CostNode> settledNodes;
    for(auto nodeId: visited)
        cost[nodeId] = MAX_DISTANCE;
    visited.clear();
    std::priority_queue<CostNode> q;
    q.emplace(source, 0);
    cost[source] = 0;
    visited.emplace_back(source);
    while(!q.empty()) {
        auto costNode = q.top();
        q.pop();
        if(costNode.cost > cost[costNode.id])
            continue;
        settledNodes.emplace_back(costNode);
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

pathFinder::CHDijkstra::CHDijkstra(pathFinder::CHGraph &graph) : graph(graph) {
    cost.reserve(graph.numberOfNodes);
    while(cost.size() <= graph.numberOfNodes)
        cost.emplace_back(MAX_DISTANCE);
}

std::optional<pathFinder::Distance> pathFinder::CHDijkstra::getShortestDistance(pathFinder::NodeId source, pathFinder::NodeId target) {
    if(source >= graph.getNodes().size() || target >= graph.getNodes().size())
        return std::nullopt;
    auto forwardLabel = shortestDistance(source, EdgeDirection::FORWARD);
    auto backwardLabel = shortestDistance(target, EdgeDirection::BACKWARD);

    HubLabels::sortLabel(forwardLabel);
    HubLabels::sortLabel(backwardLabel);
    NodeId topNode;
    return pathFinder::HubLabels::getDistance(forwardLabel, backwardLabel, topNode);
}

uint32_t pathFinder::CHDijkstra::hopCountUntilLevel(pathFinder::NodeId source, pathFinder::Level level, EdgeDirection direction) {
    uint32_t hopCount = 0;
    for(auto nodeId: visited)
        cost[nodeId] = MAX_DISTANCE;
    visited.clear();
    std::priority_queue<CostNode> q;
    q.emplace(source, 0);
    cost[source] = 0;
    visited.emplace_back(source);
    while(!q.empty()) {
        auto costNode = q.top();
        q.pop();
        if(graph.getLevel(costNode.id) >= level)
            break;
        ++hopCount;
        if(costNode.cost > cost[costNode.id])
            continue;
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
    return hopCount;
}

std::vector<pathFinder::LatLng> pathFinder::CHDijkstra::getShortestPath(pathFinder::NodeId source, pathFinder::NodeId target) {
    return std::vector<LatLng>();
}
