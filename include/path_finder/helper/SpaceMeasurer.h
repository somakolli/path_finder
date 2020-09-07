#pragma once
#include "path_finder/graphs/Graph.h"
#include <map>

namespace pathFinder {
class SpaceMeasurer {
private:
  std::map<Level, size_t> spaceConsumption;

public:
  void setSpaceConsumption(Level level, size_t space);
  [[maybe_unused]] auto getSpaceConsumption(Level level) -> size_t;
  std::string print() const;
};
} // namespace pathFinder
