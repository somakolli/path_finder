//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_DIJKSTRA_H
#define ALG_ENG_PROJECT_DIJKSTRA_H

#include <vector>
#include "Graph.h"
#include "set"
#include "PathFinderBase.h"

namespace algEng{
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
}
#endif //ALG_ENG_PROJECT_DIJKSTRA_H
