#pragma once
#include "PathFinderBase.h"
#include "path_finder/graphs/Graph.h"
#include "set"
#include <vector>

namespace pathFinder {
class Dijkstra : public PathFinderBase {
private:
  std::vector<std::optional<Distance>> costs;
  const Graph &graph;
  std::vector<NodeId> visited;
  std::vector<std::optional<NodeId>> previousNode;

public:
  explicit Dijkstra(const Graph &graph);
  std::optional<Distance> getShortestDistance(NodeId source, NodeId target);
};
} // namespace pathFinder
