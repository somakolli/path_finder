//
// Created by sokol on 07.07.20.
//
#include "path_finder/helper/Types.h"
#include <nlohmann/json.hpp>
namespace pathFinder {
void to_json(nlohmann::json &j, const CalcLabelTimingInfo calcLabelTimingInfo) {
  j = nlohmann::json{
      {"graphSearchTime", calcLabelTimingInfo.graphSearchTime},
      {"lookUpTime", calcLabelTimingInfo.lookUpTime},
      {"mergeTime", calcLabelTimingInfo.mergeTime},
  };
}
void to_json(nlohmann::json &j, const RoutingResultTimingInfo routingResult) {
  j = nlohmann::json{{"distanceTime", routingResult.distanceTime},
                     {"nodeSearchTime", routingResult.nodeSearchTime},
                     {"cellTime", routingResult.cellTime},
                     {"pathTime", routingResult.pathTime},
                     {"calcLabelTimingInfo", routingResult.calcLabelTimingInfo}};
}
std::string RoutingResultTimingInfo::toJson() const {
  nlohmann::json j;
  to_json(j, *this);
  return j.dump(1, '\t', true);
}
} // namespace pathFinder