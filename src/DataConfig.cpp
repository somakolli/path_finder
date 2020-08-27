//
// Created by sokol on 31.03.20.
//
#include <nlohmann/json.hpp>
#include <path_finder/helper/Static.h>
#include <path_finder/storage/DataConfig.h>
#include <sstream>

void pathFinder::to_json(nlohmann::json &j, const pathFinder::DataConfig &d) {
  j = nlohmann::json{
      {"graphName", d.graphName},
      {"numberOfNodes", d.numberOfNodes},
      {"numberOfEdges", d.numberOfEdges},
      {"nodes", d.nodes},
      {"calculatedUntilLevel", d.calculatedUntilLevel},
      {"forwardEdges", d.forwardEdges},
      {"forwardOffset", d.forwardOffset},
      {"backwardEdges", d.backwardEdges},
      {"backwardOffset", d.backwardOffset},
      {"forwardHublabels", d.forwardHublabels},
      {"backwardHublabels", d.backwardHublabels},
      {"forwardHublabelOffset", d.forwardHublabelOffset},
      {"backwardHublabelOffset", d.backwardHublabelOffset},
      {"gridMapEntries", d.gridMapEntries},
      {"cellIds", d.cellIds},
      {"cellIdsOffset", d.cellIdsOffset}
  };
}

void pathFinder::from_json(const nlohmann::json &j, pathFinder::DataConfig &d) {
  j.at("graphName").get_to(d.graphName);
  j.at("numberOfNodes").get_to(d.numberOfNodes);
  j.at("numberOfEdges").get_to(d.numberOfEdges);
  j.at("nodes").get_to(d.nodes);
  j.at("calculatedUntilLevel").get_to(d.calculatedUntilLevel);
  j.at("forwardEdges").get_to(d.forwardEdges);
  j.at("forwardOffset").get_to(d.forwardOffset);
  j.at("backwardEdges").get_to(d.backwardEdges);
  j.at("backwardOffset").get_to(d.backwardOffset);
  j.at("forwardHublabels").get_to(d.forwardHublabels);
  j.at("backwardHublabels").get_to(d.backwardHublabels);
  j.at("forwardHublabelOffset").get_to(d.forwardHublabelOffset);
  j.at("backwardHublabelOffset").get_to(d.backwardHublabelOffset);
  j.at("gridMapEntries").get_to(d.gridMapEntries);
  j.at("cellIds").get_to(d.cellIds);
  j.at("cellIdsOffset").get_to(d.cellIdsOffset);
}

void pathFinder::from_json(const nlohmann::json &j, BinaryFileDescription &d) {
  j.at("path").get_to(d.path);
  j.at("size").get_to(d.size);
  j.at("mmap").get_to(d.mmap);
}
void pathFinder::to_json(nlohmann::json &j, const BinaryFileDescription &d) {
  j = {{"path", d.path},
       {"size", d.size},
       {"humanReadableSize", pathFinder::Static::humanReadable(d.size)},
       {"mmap", d.mmap}};
}

std::string pathFinder::DataConfig::toJson() {
  nlohmann::json j;
  to_json(j, *this);
  return j.dump(1, '\t', true);
}


nlohmann::json pathFinder::BinaryFileDescription::toJson() const {
  nlohmann::json j;
  to_json(j, *this);
  return j.dump(1, '\t', true);
}

void pathFinder::to_json(nlohmann::json &j,const pathFinder::GridMapEntry &d){
  j = {{"lat", d.latLng.first}, {"lng", d.latLng.second}, {"begin", d.pointerPair.first}, {"end", d.pointerPair.second}};
}

void pathFinder::from_json(const nlohmann::json &j, pathFinder::GridMapEntry &d){
  j.at("lat").get_to(d.latLng.first);
  j.at("lng").get_to(d.latLng.second);
  j.at("begin").get_to(d.pointerPair.first);
  j.at("end").get_to(d.pointerPair.second);
}


void pathFinder::to_json(nlohmann::json &j,const pathFinder::GridMapEntries &d){
    j = {
        {"gridMapEntries", d.gridMapEntries},
        {"latStretchFactor", d.latStretchFactor},
        {"lngStretchFactor", d.lngStretchFactor}
  };
};

void pathFinder::from_json(const nlohmann::json &j, pathFinder::GridMapEntries &d){
    j.at("gridMapEntries").get_to(d.gridMapEntries);
    j.at("latStretchFactor").get_to(d.latStretchFactor);
    j.at("lngStretchFactor").get_to(d.lngStretchFactor);
};

void pathFinder::to_json(nlohmann::json &j, const pathFinder::DataInfo &d) {
  j = {
      {"graphName", d.graphName},
      {"timeStamp", d.timestamp}
  };
}
void pathFinder::from_json(const nlohmann::json &j, pathFinder::DataInfo &d) {
  j.at("graphName").get_to(d.graphName);
  j.at("timeStamp").get_to(d.timestamp);
}
void pathFinder::to_json(nlohmann::json &j, const pathFinder::GraphDataInfo &d) {
  to_json(j, static_cast<const DataInfo&>(d));
  j.push_back({"nodes", d.nodes});
  j.push_back({"forwardEdges", d.forwardEdges});
  j.push_back({"forwardOffset", d.forwardOffset});
  j.push_back({"backwardEdges", d.backwardEdges});
  j.push_back({"backwardOffset", d.backwardOffset});
  j.push_back({"gridMapFile", d.gridMapFile});
  j.push_back({"gridCalculated", d.gridCalculated});
  j.push_back({{"boundingBox"}, d.boundingBox});
  j.push_back({{"midPoint"}, d.midPoint});
}
void pathFinder::from_json(const nlohmann::json &j, pathFinder::GraphDataInfo &d) {
  from_json(j, static_cast<DataInfo&>(d));
  j.at("nodes").get_to(d.nodes);
  j.at("forwardEdges").get_to(d.forwardEdges);
  j.at("forwardOffset").get_to(d.forwardOffset);
  j.at("backwardEdges").get_to(d.backwardEdges);
  j.at("backwardOffset").get_to(d.backwardOffset);
  j.at("gridMapFile").get_to(d.gridMapFile);
  j.at("gridCalculated").get_to(d.gridCalculated);
  j.at("boundingBox").get_to(d.boundingBox);
  j.at("midPoint").get_to(d.midPoint);
}
void pathFinder::to_json(nlohmann::json &j, const pathFinder::HubLabelDataInfo &d) {
  to_json(j, static_cast<const DataInfo&>(d));
  j.push_back({"forwardHublabels", d.forwardHublabels});
  j.push_back({"backwardHublabels", d.backwardHublabels});
  j.push_back({"forwardHublabelOffset", d.forwardHublabelOffset});
  j.push_back({"backwardHublabelOffset", d.backwardHublabelOffset});
  j.push_back({"calculatedUntilLevel", d.calculatedUntilLevel});
  j.push_back({"maxLevel", d.maxLevel});
}
void pathFinder::from_json(const nlohmann::json &j, pathFinder::HubLabelDataInfo &d) {
  from_json(j, static_cast<DataInfo&>(d));
  j.at("forwardHublabels").get_to(d.forwardHublabels);
  j.at("backwardHublabels").get_to(d.backwardHublabels);
  j.at("forwardHublabelOffset").get_to(d.forwardHublabelOffset);
  j.at("backwardHublabelOffset").get_to(d.backwardHublabelOffset);
  j.at("calculatedUntilLevel").get_to(d.calculatedUntilLevel);
  j.at("maxLevel").get_to(d.maxLevel);
}
void pathFinder::to_json(nlohmann::json &j, const pathFinder::CellDataInfo &d) {
  to_json(j, static_cast<const DataInfo&>(d));
  j.push_back({"cellIds", d.cellIds});
  j.push_back({"cellIdsOffset", d.cellIdsOffset});
}

void pathFinder::from_json(const nlohmann::json &j, pathFinder::CellDataInfo &d) {
  from_json(j, static_cast<DataInfo&>(d));
  j.at("cellIds").get_to(d.cellIds);
  j.at("cellIdsOffset").get_to(d.cellIdsOffset);
}

void pathFinder::to_json(nlohmann::json &j, const pathFinder::HybridPfDataInfo &d){
  j = nlohmann::json{
      {"hubLabelFolder", d.hubLabelFolder},
      {"cellIdFolder", d.cellIdFolder},
      {"graphFolder", d.graphFolder},
      {"labelsUntilLevel", d.labelsUntilLevel},
      {"cellIdsCalculated", d.cellIdsCalculated},
      {"hubLabelsCalculated", d.hubLabelsCalculated},
  };
}

void pathFinder::from_json(const nlohmann::json &j, pathFinder::HybridPfDataInfo &d){
    j.at("hubLabelFolder").get_to(d.hubLabelFolder);
    j.at("cellIdFolder").get_to(d.cellIdFolder);
    j.at("graphFolder").get_to(d.graphFolder);
    j.at("labelsUntilLevel").get_to(d.labelsUntilLevel);
    j.at("cellIdsCalculated").get_to(d.cellIdsCalculated);
    j.at("hubLabelsCalculated").get_to(d.hubLabelsCalculated);
}
