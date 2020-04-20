//
// Created by sokol on 02.12.19.
//

#ifndef ALG_ENG_PROJECT_PATHFINDERBASE_H
#define ALG_ENG_PROJECT_PATHFINDERBASE_H

#include "Graph.h"
#include <optional>

namespace pathFinder {
class PathFinderBase {
public:
  virtual std::optional<Distance> getShortestDistance(NodeId source,
                                                      NodeId target);
  virtual std::vector<LatLng> getShortestPath(NodeId source, NodeId target);
};
} // namespace pathFinder

#endif // ALG_ENG_PROJECT_PATHFINDERBASE_H
