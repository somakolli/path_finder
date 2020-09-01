#pragma once
#include "path_finder/graphs/Graph.h"
#include "path_finder/helper/Types.h"
#include <optional>

namespace pathFinder {
class PathFinderBase {
public:
  virtual auto getShortestPath(NodeId source, NodeId target) -> RoutingResult;
  virtual auto getShortestPath(LatLng source, LatLng target) -> RoutingResult;
};
} // namespace pathFinder
