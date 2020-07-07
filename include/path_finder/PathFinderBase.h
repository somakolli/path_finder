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
  virtual std::vector<LatLng> getShortestPath(NodeId source, NodeId target, std::optional<Distance>& distance, std::vector<unsigned int>* cellIds);
  virtual std::vector<LatLng> getShortestPath(LatLng source, LatLng target, std::optional<Distance>& distance, std::vector<unsigned int>* cellIds);
};
} // namespace pathFinder

#endif // ALG_ENG_PROJECT_PATHFINDERBASE_H
