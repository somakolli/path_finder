//
// Created by sokol on 07.07.20.
//

#ifndef MASTER_ARBEIT_TYPES_H
#define MASTER_ARBEIT_TYPES_H
#include "CHGraph.h"
#include "CellIdStore.h"
#include "Graph.h"
#include "MmapVector.h"
#include <vector>
namespace pathFinder {
using costNodeVec_t = std::vector<CostNode>;
using RamGraph = CHGraph<std::vector, std::vector<NodeId>>;
using RamCellIdStore = CellIdStore<std::vector, std::vector<OffsetElement>, unsigned int>;
using RamHubLabelStore = HubLabelStore<std::vector, std::vector<OffsetElement>>;
using MMapGraph = CHGraph<MmapVector, MmapVector<NodeId>>;
using MMapCellIdStore = CellIdStore<MmapVector, MmapVector<OffsetElement>, unsigned int>;
using MMapHubLabelStore = HubLabelStore<MmapVector, MmapVector<OffsetElement>>;
struct RoutingResult {
    std::vector<LatLng> path;
    std::vector<CellId_t> cellIds;
    Distance distance;
};
}
#endif // MASTER_ARBEIT_TYPES_H
