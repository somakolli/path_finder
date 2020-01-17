//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_DIJKSTRA_H
#define ALG_ENG_PROJECT_DIJKSTRA_H

#include <vector>
#include "Graph.h"
#include "set"
#include "PathFinderBase.h"

namespace pathFinder{
    template <typename Graph>
class Dijkstra : public PathFinderBase{
private:
	std::vector<std::optional<Distance>> costs;
	const Graph& graph;
	std::vector<NodeId> visited;
	std::vector<std::optional<NodeId>> previousNode;
	NodeId previousSource = graph.numberOfNodes + 1;
public:
    explicit Dijkstra(const Graph &graph);
	std::optional<Distance> getShortestDistance(NodeId source, NodeId target);
	std::vector<LatLng> getShortestPath(NodeId source, NodeId target);
};

    template<typename Graph>
    std::optional<Distance> Dijkstra<Graph>::getShortestDistance(NodeId source, NodeId target) {
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

    template<typename Graph>
    Dijkstra<Graph>::Dijkstra(const Graph &graph) {
        costs.reserve(graph.numberOfNodes);
        previousNode.reserve(graph.numberOfNodes);
        while (costs.size() < graph.numberOfNodes){
            costs.emplace_back(MAX_DISTANCE);
            previousNode.emplace_back(std::nullopt);
        }
    }

    template<typename Graph>
    std::vector<LatLng> Dijkstra<Graph>::getShortestPath(NodeId source, NodeId target) {
        return std::vector<LatLng>();
    }
}
#endif //ALG_ENG_PROJECT_DIJKSTRA_H
