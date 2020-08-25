//
// Created by sokol on 20.04.20.
//

#ifndef MASTER_ARBEIT_GRID_H
#define MASTER_ARBEIT_GRID_H
#include <cmath>
#include <map>
#include <utility>
namespace pathFinder {
class Grid {
public:
  void createForGraph(CHGraph& graph, double latStretchFactor, double lngStretchFactor);
private:
  std::map<LatLng, std::pair<NodeId, NodeId>> m_grid;

};
} // namespace pathFinder
#endif // MASTER_ARBEIT_GRID_H
