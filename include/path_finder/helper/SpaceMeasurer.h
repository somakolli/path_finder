#pragma once
#include "path_finder/graphs/Graph.h"
#include <map>

namespace pathFinder {
class SpaceMeasurer {
private:
  std::map<Level, size_t> spaceConsumption;

public:
  void setSpaceConsumption(Level level, size_t space);
  [[maybe_unused]] size_t getSpaceConsumption(Level level);
};
} // namespace pathFinder
