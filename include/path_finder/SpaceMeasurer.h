//
// Created by sokol on 19.03.20.
//

#ifndef MASTER_ARBEIT_SPACEMEASURER_H
#define MASTER_ARBEIT_SPACEMEASURER_H
#include "Graph.h"
#include <map>

namespace pathFinder {
class SpaceMeasurer {
private:
  std::map<Level, size_t> spaceConsumption;

public:
  void setSpaceConsumption(Level level, size_t space);
  size_t getSpaceConsumption(Level level);
};
} // namespace pathFinder

#endif // MASTER_ARBEIT_SPACEMEASURER_H
