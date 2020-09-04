//
// Created by sokol on 19.03.20.
//

#include "path_finder/helper/SpaceMeasurer.h"
#include <sstream>

void pathFinder::SpaceMeasurer::setSpaceConsumption(pathFinder::Level nodeId, size_t space){
  spaceConsumption[nodeId] = space;
}

[[maybe_unused]] auto pathFinder::SpaceMeasurer::getSpaceConsumption(pathFinder::Level level) -> size_t {
  return spaceConsumption[level];
}
auto pathFinder::SpaceMeasurer::print() const -> std::string {
  std::stringstream levelStream;
  std::stringstream spaceStream;
  levelStream << "level = [";
  spaceStream << "space = [";
  bool first = true;
  for(auto [level, space] : spaceConsumption) {
    if(!first) {
      levelStream << ',';
      spaceStream << ',';
    }
    first = false;
    levelStream << level;
    spaceStream << space;
  }
  return levelStream.str() + "]" +  '\n' + spaceStream.str() + ']';
}
