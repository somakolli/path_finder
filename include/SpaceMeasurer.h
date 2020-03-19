//
// Created by sokol on 19.03.20.
//

#ifndef MASTER_ARBEIT_SPACEMEASURER_H
#define MASTER_ARBEIT_SPACEMEASURER_H
#include <map>
#include "Graph.h"

namespace pathFinder{
class SpaceMeasurer {
private:
    std::map<Level, size_t> spaceConsumption;
public:
    void setSpaceConsumption(Level level, size_t space);
    size_t getSpaceConsumption(Level level);
};
}

#endif //MASTER_ARBEIT_SPACEMEASURER_H
