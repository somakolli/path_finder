//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_DIJKSTRA_H
#define ALG_ENG_PROJECT_DIJKSTRA_H

#include "Graph.h"
#include "PathFinderBase.h"
#include "set"
#include <vector>

namespace pathFinder {
class Dijkstra : public PathFinderBase {
private:
  std::vector<std::optional<Distance>> costs;
  const Graph &graph;
  std::vector<NodeId> visited;
  std::vector<std::optional<NodeId>> previousNode;
  NodeId previousSource = graph.numberOfNodes + 1;

public:
  explicit Dijkstra(const Graph &graph);
  std::optional<Distance> getShortestDistance(NodeId source, NodeId target);
};
} // namespace pathFinder
#endif // ALG_ENG_PROJECT_DIJKSTRA_H
