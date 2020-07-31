//
// Created by sokol on 02.12.19.
//

#ifndef ALG_ENG_PROJECT_PATHFINDERBASE_H
#define ALG_ENG_PROJECT_PATHFINDERBASE_H

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

#endif // ALG_ENG_PROJECT_PATHFINDERBASE_H
