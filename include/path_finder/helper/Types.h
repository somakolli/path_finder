//
// Created by sokol on 07.07.20.
//

#ifndef MASTER_ARBEIT_TYPES_H
#define MASTER_ARBEIT_TYPES_H
#include "path_finder/graphs/CHGraph.h"
#include "path_finder/graphs/Graph.h"
#include "path_finder/storage/CellIdStore.h"
#include "path_finder/storage/MmapVector.h"
#include <vector>
namespace pathFinder {
using costNodeVec_t = std::vector<CostNode>;
using RamGraph = CHGraph<std::vector, std::vector<NodeId>>;
using RamCellIdStore = CellIdStore<CellId_t>;
using RamHubLabelStore = HubLabelStore;
using MMapGraph = CHGraph<MmapVector, MmapVector<NodeId>>;
using MMapCellIdStore = CellIdStore<CellId_t>;
using MMapHubLabelStore = HubLabelStore;
struct CalcLabelTimingInfo{
  double mergeTime = 0;
  double graphSearchTime = 0;
  double lookUpTime = 0;

  void operator+=(const CalcLabelTimingInfo& other) {
    mergeTime += other.mergeTime;
    graphSearchTime += other.graphSearchTime;
    lookUpTime += other.lookUpTime;
  }
  void operator/=(double other) {
    mergeTime /= other;
    lookUpTime /= other;
    graphSearchTime /= other;
  }
  friend std::ostream& operator<<(std::ostream& os, const CalcLabelTimingInfo& calcLabelTimingInfo) {
    os << "mergeTime: " << calcLabelTimingInfo.mergeTime << '\n';
    os << "lookUpTime: " << calcLabelTimingInfo.lookUpTime << '\n';
    os << "graphSearchTime: " << calcLabelTimingInfo.graphSearchTime << '\n';
    return os;
  }
};

struct RoutingResult {
    std::vector<uint32_t> edgeIds;
    std::vector<LatLng> path;
    std::vector<CellId_t> cellIds;
    Distance distance;
    double distanceTime;
    double pathTime = 0;
    double cellTime = 0;
    double nodeSearchTime = 0;
    CalcLabelTimingInfo calcLabelTimingInfo{};
};
}
#endif // MASTER_ARBEIT_TYPES_H
