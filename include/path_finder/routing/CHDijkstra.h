#pragma once
#include "PathFinderBase.h"
#include "path_finder/graphs/CHGraph.h"
#include "path_finder/helper/Static.h"
#include <queue>
#include <set>

namespace pathFinder {
class CHDijkstra : public PathFinderBase {
public:
  explicit CHDijkstra(std::shared_ptr<CHGraph> graph);
  auto getShortestDistance(NodeId source, NodeId target) -> std::optional<Distance>;
  auto shortestDistance(NodeId source, EdgeDirection direction) -> std::vector<CostNode>;
  [[nodiscard]] auto getNodeCount() const -> size_t;
private:
  std::vector<Distance> cost;
  std::vector<NodeId> visited;
  std::shared_ptr<CHGraph> graph;
};
} // namespace pathFinder
