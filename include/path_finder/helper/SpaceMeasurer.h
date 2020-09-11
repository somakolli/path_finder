#pragma once
#include "path_finder/graphs/Graph.h"
#include <map>

namespace pathFinder {
class SpaceMeasurer {
private:
  std::map<Level, size_t> spaceConsumption;

public:
  SpaceMeasurer();
  explicit SpaceMeasurer(const std::vector<size_t>& spaceVector);
  void setSpaceConsumption(Level level, size_t space);
  [[maybe_unused]] auto getSpaceConsumption(Level level) -> size_t;
  [[nodiscard]] auto print() const -> std::string;
  auto toVector() const -> std::vector<size_t>;

};
void to_json(nlohmann::json& j, const SpaceMeasurer& spaceMeasurer);
void from_json(const nlohmann::json& j, SpaceMeasurer& spaceMeasurer);
} // namespace pathFinder
