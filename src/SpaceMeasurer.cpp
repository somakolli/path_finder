//
// Created by sokol on 19.03.20.
//

#include "../include/SpaceMeasurer.h"

void pathFinder::SpaceMeasurer::setSpaceConsumption(pathFinder::Level nodeId,
                                                    size_t space) {
  spaceConsumption[nodeId] = space;
}

size_t pathFinder::SpaceMeasurer::getSpaceConsumption(pathFinder::Level level) {
  return spaceConsumption[level];
}
