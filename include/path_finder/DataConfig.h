//
// Created by sokol on 31.03.20.
//

#ifndef MASTER_ARBEIT_DATACONFIG_H
#define MASTER_ARBEIT_DATACONFIG_H

#include "Graph.h"
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
  std::pair<Lat, Lng> latLng;
  std::pair<NodeId, NodeId> pointerPair;
};

void from_json(const nlohmann::json &j, BinaryFileDescription &d);
void to_json(nlohmann::json &j, const BinaryFileDescription &d);
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

  static DataConfig getFromFile(const std::string &filepath);
};

void to_json(nlohmann::json &j, const pathFinder::DataConfig &d);

void from_json(const nlohmann::json &j, pathFinder::DataConfig &d);

void to_json(nlohmann::json &j,const pathFinder::GridMapEntry &d);

void from_json(const nlohmann::json &j, pathFinder::GridMapEntry &d);
} // namespace pathFinder
#endif // MASTER_ARBEIT_DATACONFIG_H
