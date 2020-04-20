//
// Created by sokol on 31.03.20.
//
#include "../include/DataConfig.h"
#include <nlohmann/json.hpp>
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
}

void pathFinder::from_json(const nlohmann::json &j, BinaryFileDescription &d) {
  j.at("path").get_to(d.path);
  j.at("size").get_to(d.size);
  j.at("mmap").get_to(d.mmap);
}
void pathFinder::to_json(nlohmann::json &j, const BinaryFileDescription &d) {
  j = {{"path", d.path}, {"size", d.size}, {"mmap", d.mmap}};
}

std::string pathFinder::DataConfig::toJson() {
  nlohmann::json j;
  to_json(j, *this);
  return j.dump();
}

pathFinder::DataConfig
pathFinder::DataConfig::getFromFile(const std::string &configString) {
  auto j = nlohmann::json::parse(configString);
  return j.get<pathFinder::DataConfig>();
}

nlohmann::json pathFinder::BinaryFileDescription::toJson() const {
  nlohmann::json j;
  to_json(j, *this);
  return j.dump();
}
