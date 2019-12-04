//
// Created by sokol on 02.10.19.
//
#include <queue>
#include "../include/Dijkstra.h"

algEng::Dijkstra::Dijkstra(const algEng::Graph &graph) : graph(graph) {
	costs.reserve(graph.numberOfNodes);
	previousNode.reserve(graph.numberOfNodes);
	while (costs.size() < graph.numberOfNodes){
        costs.emplace_back(MAX_DISTANCE);
        previousNode.emplace_back(std::nullopt);
    }
}

std::optional<Distance> algEng::Dijkstra::getShortestDistance(algEng::NodeId source, algEng::NodeId target) {
    if(source >= graph.numberOfNodes || target >= graph.numberOfNodes)
        return std::nullopt;
	//clean up distances
	for(auto& nodeId: visited){
        costs[nodeId] = MAX_DISTANCE;
    }
	visited.clear();
	visited.emplace_back(source);
	costs[source] = 0;
	std::priority_queue<CostNode> q;
	q.emplace(source, 0);
	while (!q.empty()) {
		const auto costNode = q.top();
		q.pop();
		if(costNode.id == target) {
			return costNode.cost;
		}
		for(const auto& edge : graph.edgesFor(costNode.id)) {
			Distance currentCost = costNode.cost + edge.distance;
			if(currentCost < costs[edge.target]) {
				costs[edge.target] = currentCost;
				q.emplace(edge.target, currentCost);
				visited.emplace_back(edge.target);
			}
		}
	}
	return costs[target];
}

std::vector<algEng::LatLng> algEng::Dijkstra::getShortestPath(algEng::NodeId source, algEng::NodeId target) {
    return std::vector<LatLng>();
}
