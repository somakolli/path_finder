//
// Created by sokol on 07.07.20.
//

#ifndef MASTER_ARBEIT_TYPES_H
#define MASTER_ARBEIT_TYPES_H
#include "CellIdStore.h"
#include "Graph.h"
#include <vector>
namespace pathFinder {
using costNodeVec_t = std::vector<CostNode>;
struct RoutingResult {
    std::vector<LatLng> path;
    std::vector<CellId_t> cellIds;
    Distance distance;
};
}
#endif // MASTER_ARBEIT_TYPES_H