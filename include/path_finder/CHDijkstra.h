//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHDIJKSTRA_H
#define ALG_ENG_PROJECT_CHDIJKSTRA_H

#include "CHGraph.h"
#include "PathFinderBase.h"
#include "Static.h"
#include <queue>
#include <set>

namespace pathFinder {
template <typename Graph> class CHDijkstra : public PathFinderBase {
public:
  explicit CHDijkstra(Graph &graph);
  std::optional<Distance> getShortestDistance(NodeId source,
                                              NodeId target);
  std::vector<CostNode> shortestDistance(NodeId source,
                                         EdgeDirection direction);
  std::vector<LatLng> getShortestPath(NodeId source, NodeId target);

private:
  std::vector<Distance> cost;
  std::vector<NodeId> visited;
  Graph &graph;
};
template <typename Graph>
std::vector<pathFinder::CostNode>
pathFinder::CHDijkstra<Graph>::shortestDistance(
    pathFinder::NodeId source, pathFinder::EdgeDirection direction) {
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
    for (const auto &edge : graph.edgesFor(costNode.id, direction)) {
      if (graph.getLevel(edge.source) > graph.getLevel(edge.target))
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
template <typename Graph>
pathFinder::CHDijkstra<Graph>::CHDijkstra(Graph &graph) : graph(graph) {
  cost.reserve(graph.numberOfNodes);
  while (cost.size() <= graph.numberOfNodes)
    cost.emplace_back(MAX_DISTANCE);
}
template <typename Graph>
std::optional<pathFinder::Distance>
pathFinder::CHDijkstra<Graph>::getShortestDistance(pathFinder::NodeId source,
                                                   pathFinder::NodeId target) {
  if (source >= graph.getNodes().size() || target >= graph.getNodes().size())
    return std::nullopt;
  auto forwardLabel = shortestDistance(source, EdgeDirection::FORWARD);
  auto backwardLabel = shortestDistance(target, EdgeDirection::BACKWARD);

  Static::sortLabel(forwardLabel);
  Static::sortLabel(backwardLabel);
  NodeId topNode;
  return Static::getShortestDistance(MyIterator<CostNode *>(forwardLabel.begin().base(),
                                                 forwardLabel.end().base()),
                          MyIterator<CostNode *>(backwardLabel.begin().base(),
                                                 backwardLabel.end().base()),
                          topNode);
}
template <typename Graph>
std::vector<pathFinder::LatLng>
pathFinder::CHDijkstra<Graph>::getShortestPath(pathFinder::NodeId source,
                                               pathFinder::NodeId target) {
  return std::vector<LatLng>();
}
} // namespace pathFinder

#endif // ALG_ENG_PROJECT_CHDIJKSTRA_H
