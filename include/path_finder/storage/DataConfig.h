#pragma once
#include "path_finder/graphs/Graph.h"
#include <nlohmann/json.hpp>
#include <string>

namespace pathFinder {

struct BinaryFileDescription {
  std::string path;
  size_t size;
  bool mmap;
};
struct GridMapEntry {
  std::pair<int, int> latLng;
  std::pair<NodeId, NodeId> pointerPair;
};
struct GridMapEntries {
  double latStretchFactor;
  double lngStretchFactor;
  std::vector<GridMapEntry> gridMapEntries;
};

void from_json(const nlohmann::json &j, BinaryFileDescription &d);
void to_json(nlohmann::json &j, const BinaryFileDescription &d);

struct DataInfo {
  std::string graphName;
  std::string timestamp;
};

struct GraphDataInfo : DataInfo {
  BinaryFileDescription nodes;
  bool gridCalculated;
  BinaryFileDescription forwardEdges;
  BinaryFileDescription forwardOffset;
  BinaryFileDescription backwardEdges;
  BinaryFileDescription backwardOffset;
  std::string gridMapFile;
  BoundingBox boundingBox;
  LatLng midPoint;
};

struct HubLabelDataInfo : DataInfo {
  size_t calculatedUntilLevel = -1;
  size_t maxLevel = -1;
  BinaryFileDescription forwardHublabels;
  BinaryFileDescription forwardHublabelOffset;
  BinaryFileDescription backwardHublabels;
  BinaryFileDescription backwardHublabelOffset;
};

struct CellDataInfo : DataInfo {
  BinaryFileDescription cellIds;
  BinaryFileDescription cellIdsOffset;
};

struct HybridPfDataInfo {
  std::string graphFolder;
  std::string hubLabelFolder;
  std::string cellIdFolder;
  int labelsUntilLevel = -1;
  bool cellIdsCalculated = false;
  bool hubLabelsCalculated = false;
};

struct DataConfig {
  std::string graphName;

  size_t numberOfNodes;
  size_t numberOfEdges;

  size_t calculatedUntilLevel;

  BinaryFileDescription nodes;
  BinaryFileDescription forwardEdges;
  BinaryFileDescription forwardOffset;
  BinaryFileDescription backwardEdges;
  BinaryFileDescription backwardOffset;
  BinaryFileDescription forwardHubLabels;
  BinaryFileDescription forwardHubLabelOffset;
  BinaryFileDescription backwardHubLabels;
  BinaryFileDescription backwardHubLabelOffset;
  BinaryFileDescription cellIds;
  BinaryFileDescription cellIdsOffset;
  std::vector<GridMapEntry> gridMapEntries;

  auto toJson() -> std::string;
  template <typename T>
  static auto getFromFile(const std::string &filepath) -> T;
};

template <typename T>
auto pathFinder::DataConfig::getFromFile(const std::string &filepath) -> T {
  auto j = nlohmann::json::parse(filepath);
  return j.get<T>();
}

void to_json(nlohmann::json &j, const pathFinder::DataConfig &d);

void from_json(const nlohmann::json &j, pathFinder::DataConfig &d);

void to_json(nlohmann::json &j, const pathFinder::GridMapEntry &d);

void from_json(const nlohmann::json &j, pathFinder::GridMapEntry &d);

void to_json(nlohmann::json &j, const pathFinder::GridMapEntries &d);

void from_json(const nlohmann::json &j, pathFinder::GridMapEntries &d);

void to_json(nlohmann::json &j, const pathFinder::DataInfo &d);

void from_json(const nlohmann::json &j, pathFinder::DataInfo &d);

void to_json(nlohmann::json &j, const pathFinder::GraphDataInfo &d);

void from_json(const nlohmann::json &j, pathFinder::GraphDataInfo &d);

void to_json(nlohmann::json &j, const pathFinder::HubLabelDataInfo &d);

void from_json(const nlohmann::json &j, pathFinder::HubLabelDataInfo &d);

void to_json(nlohmann::json &j, const pathFinder::CellDataInfo &d);

void from_json(const nlohmann::json &j, pathFinder::CellDataInfo &d);

void to_json(nlohmann::json &j, const pathFinder::HybridPfDataInfo &d);

void from_json(const nlohmann::json &j, pathFinder::HybridPfDataInfo &d);

} // namespace pathFinder
