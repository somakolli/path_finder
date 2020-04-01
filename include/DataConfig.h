//
// Created by sokol on 31.03.20.
//

#ifndef MASTER_ARBEIT_DATACONFIG_H
#define MASTER_ARBEIT_DATACONFIG_H

#include <string>
#include <nlohmann/json.hpp>

namespace pathFinder {

struct BinaryFileDescription {
    std::string path;
    size_t size;
    bool mmap;
    nlohmann::json toJson() const;

};
void from_json(const nlohmann::json &j, BinaryFileDescription &d);
void to_json(nlohmann::json &j,const BinaryFileDescription &d);
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

    std::string toJson();

    static DataConfig getFromFile(const std::string &filepath);
};

void to_json(nlohmann::json &j, const pathFinder::DataConfig &d);

void from_json(const nlohmann::json &j, pathFinder::DataConfig &d);
}
#endif //MASTER_ARBEIT_DATACONFIG_H
