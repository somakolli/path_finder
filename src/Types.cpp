//
// Created by sokol on 07.07.20.
//
#include "path_finder/helper/Types.h"
#include <nlohmann/json.hpp>
namespace pathFinder {
void to_json(nlohmann::json& j, const CalcLabelTimingInfo calcLabelTimingInfo) {
  j = nlohmann::json {
      {"graphSearchTime", calcLabelTimingInfo.graphSearchTime},
      {"lookUpTime", calcLabelTimingInfo.lookUpTime},
      {"mergeTime", calcLabelTimingInfo.mergeTime},
  };
}
void to_json(nlohmann::json& j, const RoutingResultTimingInfo routingResult) {
  j = nlohmann::json {
      {"distanceTime", routingResult.distanceTime},
      {"nodeSearchTime", routingResult.nodeSearchTime},
      {"cellTime", routingResult.cellTime},
      {"pathTime", routingResult.pathTime},
      {"calcLabelTimingInfo", routingResult.calcLabelTimingInfo}
  };
}
void from_json(const nlohmann::json &j, BoundingBox &boundingBox) {
  j.at("north").get_to(boundingBox.north);
  j.at("east").get_to(boundingBox.east);
  j.at("south").get_to(boundingBox.south);
  j.at("west").get_to(boundingBox.west);
}
void to_json(nlohmann::json &j, BoundingBox boundingBox) {
  j = nlohmann::json {
      {"north", boundingBox.north},
      {"east", boundingBox.east},
      {"south", boundingBox.south},
      {"west", boundingBox.west}
  };
}
void to_json(nlohmann::json &j, LatLng latLng) {
  j = nlohmann::json {
      {"lat", latLng.lat},
      {"lng", latLng.lng}
  };
}
void from_json(const nlohmann::json &j, LatLng &latLng) {
  j.at("lat").get_to(latLng.lat);
  j.at("lng").get_to(latLng.lng);
}
std::string CalcLabelTimingInfo::toJson() const{
  nlohmann::json j;
  to_json(j, *this);
  return j.dump(1, '\t', true);
}
std::string RoutingResultTimingInfo::toJson() const{
  nlohmann::json j;
  to_json(j, *this);
  return j.dump(1, '\t', true);
}
}