//
// Created by sokol on 03.10.19.
//
#include "path_finder/routing/CHDijkstra.h"
namespace pathFinder {
pathFinder::CHDijkstra::CHDijkstra(std::shared_ptr<CHGraph> graph) : graph(graph) {
  cost.reserve(graph->getNumberOfNodes());
  while (cost.size() <= graph->getNumberOfNodes())
    cost.emplace_back(MAX_DISTANCE);
}

auto pathFinder::CHDijkstra::shortestDistance(pathFinder::NodeId source,
                                                                           pathFinder::EdgeDirection direction) -> std::vector<pathFinder::CostNode> {
  std::vector<CostNode> settledNodes;
  for (auto nodeId : visited)
    cost[nodeId] = MAX_DISTANCE;
  visited.clear();
  std::priority_queue<CostNode> q;
  q.emplace(source, 0, source);
  cost[source] = 0;
  visited.emplace_back(source);
  settledNodes.emplace_back(source, 0, source);
  while (!q.empty()) {
    auto costNode = q.top();
    q.pop();
    if (costNode.cost > cost[costNode.id])
      continue;
    settledNodes.push_back(costNode);
    for (const auto &edge : graph->edgesFor(costNode.id, direction)) {
      if (graph->getLevel(edge.source) > graph->getLevel(edge.target))
        continue;
      auto addedCost = costNode.cost + edge.distance;
      if (addedCost < cost[edge.target]) {
        visited.emplace_back(edge.target);
        cost[edge.target] = addedCost;
        q.emplace(edge.target, addedCost, edge.source);
      }
    }
  }
  return settledNodes;
}
auto pathFinder::CHDijkstra::getShortestDistance(pathFinder::NodeId source,
                                                                                pathFinder::NodeId target) -> std::optional<pathFinder::Distance> {
  if (source >= graph->getNumberOfNodes() || target >= graph->getNumberOfNodes())
    return std::nullopt;
  auto forwardLabel = shortestDistance(source, EdgeDirection::FORWARD);
  auto backwardLabel = shortestDistance(target, EdgeDirection::BACKWARD);

  Static::sortLabel(forwardLabel);
  Static::sortLabel(backwardLabel);
  NodeId topNode;
  return Static::getShortestDistance(
      MyIterator<volatile const CostNode *>(forwardLabel.begin().base(), forwardLabel.end().base()),
      MyIterator<volatile const CostNode *>(backwardLabel.begin().base(), backwardLabel.end().base()), topNode);
}
} // namespace pathFinder