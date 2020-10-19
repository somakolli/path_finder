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
pathFinder::SpaceMeasurer::SpaceMeasurer(const std::vector<size_t>& spaceVector) {
  for(int i = 0; i < spaceVector.size(); ++i) {
    setSpaceConsumption(i, spaceVector[i]);
  }
}
auto pathFinder::SpaceMeasurer::toVector()const  -> std::vector<size_t> {
  std::vector<size_t> spaceVec(spaceConsumption.size(), 0);
  for (auto [level, size] : spaceConsumption) {
	if (spaceVec.size() <= level) {
		spaceVec.resize(level+1, 0);
	}
    spaceVec.at(level) = size;
  }
  return spaceVec;
}
pathFinder::SpaceMeasurer::SpaceMeasurer() = default;
void pathFinder::to_json(nlohmann::json &j, const pathFinder::SpaceMeasurer &spaceMeasurer) {
  j = nlohmann::json{
      {"spaceConsumption", spaceMeasurer.toVector()}
  };
}
void pathFinder::from_json(const nlohmann::json &j, pathFinder::SpaceMeasurer &spaceMeasurer) {
  auto test = j.at("spaceConsumption");
  auto vector = test.get<std::vector<size_t>>();
  spaceMeasurer = SpaceMeasurer(vector);
}
