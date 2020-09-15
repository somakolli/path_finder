//
// Created by sokol on 03.10.19.
//
#include "path_finder/routing/CHDijkstra.h"
namespace pathFinder {
CHDijkstra::CHDijkstra(std::shared_ptr<CHGraph> graph) : graph(graph) {
  cost.reserve(graph->getNumberOfNodes());
  while (cost.size() <= graph->getNumberOfNodes())
    cost.emplace_back(MAX_DISTANCE);
}

auto CHDijkstra::shortestDistance(pathFinder::NodeId source,
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
    Level sourceLevel = graph->getLevel(costNode.id);

    bool stallNode = false;
    // stall on demand
    for(const auto &incomingEdge : graph->edgesFor(costNode.id, static_cast<EdgeDirection>(!direction))) {
      if(sourceLevel > graph->getLevel(incomingEdge.target))
        continue;
      // make cost size larger to avoid integer overflow in the case that cost is max
      size_t targetCost = cost[incomingEdge.target];
      if(targetCost + incomingEdge.distance < costNode.cost)
        stallNode = true;
    }
    if(stallNode)
      continue;

    for (const auto &edge : graph->edgesFor(costNode.id, direction)) {
      if (sourceLevel > graph->getLevel(edge.target))
        continue;
      
      auto addedCost = costNode.cost + edge.distance;
      auto& targetCost = cost[edge.target];
      if (addedCost < targetCost) {
        visited.emplace_back(edge.target);
        targetCost = addedCost;
        q.emplace(edge.target, addedCost, edge.source);
      }
    }
  }
  return settledNodes;
}
auto CHDijkstra::getShortestDistance(pathFinder::NodeId source,
                   pathFinder::NodeId target) -> std::optional<pathFinder::Distance> {
  if (source >= graph->getNumberOfNodes() || target >= graph->getNumberOfNodes())
    return std::nullopt;
  auto forwardLabel = shortestDistance(source, EdgeDirection::FORWARD);
  auto backwardLabel = shortestDistance(target, EdgeDirection::BACKWARD);

  Static::sortLabel(forwardLabel);
  Static::sortLabel(backwardLabel);
  NodeId topNode;
  return Static::getShortestDistance(
      MyIterator<const CostNode *>(forwardLabel.begin().base(), forwardLabel.end().base()),
      MyIterator<const CostNode *>(backwardLabel.begin().base(), backwardLabel.end().base()),
          topNode);
}
auto CHDijkstra::getNodeCount() const -> size_t { return graph->getNumberOfNodes();}
} // namespace pathFinder