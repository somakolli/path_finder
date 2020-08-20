//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHDIJKSTRA_H
#define ALG_ENG_PROJECT_CHDIJKSTRA_H

#include "PathFinderBase.h"
#include "path_finder/graphs/CHGraph.h"
#include "path_finder/helper/Static.h"
#include <queue>
#include <set>

namespace pathFinder {
class CHDijkstra : public PathFinderBase {
public:
  explicit CHDijkstra(std::shared_ptr<CHGraph> graph);
  std::optional<Distance> getShortestDistance(NodeId source,
                                              NodeId target);
  std::vector<CostNode> shortestDistance(NodeId source,
                                         EdgeDirection direction);

private:
  std::vector<Distance> cost;
  std::vector<NodeId> visited;
  std::shared_ptr<CHGraph> graph;
};
} // namespace pathFinder

#endif // ALG_ENG_PROJECT_CHDIJKSTRA_H
