#pragma once
#include "path_finder/graphs/Graph.h"
#include "path_finder/helper/Types.h"
#include <optional>

namespace pathFinder {
class PathFinderBase {
public:
  virtual RoutingResult getShortestPath(NodeId source, NodeId target);
  virtual RoutingResult getShortestPath(LatLng source, LatLng target);
};
} // namespace pathFinder
