//
// Created by sokol on 31.03.20.
//

#ifndef MASTER_ARBEIT_DATACONFIG_H
#define MASTER_ARBEIT_DATACONFIG_H

#include "path_finder/graphs/Graph.h"
#include <nlohmann/json.hpp>
#include <string>

namespace pathFinder {

struct BinaryFileDescription {
  std::string path;
  size_t size;
  bool mmap;
  nlohmann::json toJson() const;
};
struct GridMapEntry {
  std::pair<int, int> latLng;
  std::pair<NodeId, NodeId> pointerPair;
};

void from_json(const nlohmann::json &j, BinaryFileDescription &d);
void to_json(nlohmann::json &j, const BinaryFileDescription &d);

struct DataInfo {
  std::string graphName;
  std::string timestamp;
};

struct GraphDataInfo  : DataInfo{
  BinaryFileDescription nodes;
  bool gridCalculated;
  BinaryFileDescription forwardEdges;
  BinaryFileDescription forwardOffset;
  BinaryFileDescription backwardEdges;
  BinaryFileDescription backwardOffset;
  std::vector<GridMapEntry> gridMapEntries;
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
  BinaryFileDescription forwardHublabels;
  BinaryFileDescription forwardHublabelOffset;
  BinaryFileDescription backwardHublabels;
  BinaryFileDescription backwardHublabelOffset;
  BinaryFileDescription cellIds;
  BinaryFileDescription cellIdsOffset;
  std::vector<GridMapEntry> gridMapEntries;

  std::string toJson();
  template <typename T>
  static T getFromFile(const std::string &filepath);
};

template <typename T>
T pathFinder::DataConfig::getFromFile(const std::string &filepath) {
  auto j = nlohmann::json::parse(filepath);
  return j.get<T>();
}

void to_json(nlohmann::json &j, const pathFinder::DataConfig &d);

void from_json(const nlohmann::json &j, pathFinder::DataConfig &d);

void to_json(nlohmann::json &j,const pathFinder::GridMapEntry &d);

void from_json(const nlohmann::json &j, pathFinder::GridMapEntry &d);

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
#endif // MASTER_ARBEIT_DATACONFIG_H
