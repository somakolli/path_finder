//
// Created by sokol on 20.04.20.
//

#ifndef MASTER_ARBEIT_GRID_H
#define MASTER_ARBEIT_GRID_H
#include "CHGraph.h"
#include "path_finder/graphs/CHGraph.h"
#include "path_finder/storage/GraphReader.h"
#include <cmath>
#include <map>
#include <utility>
namespace pathFinder {
class Grid {
  using GridKey = std::pair<Lat, Lng>;
  CHGraph& graph;
  int gridSizeX;
  int gridSizeY;

public:
  std::map<GridKey, std::vector<CHNode>> mapGrid;
  std::map<GridKey, std::pair<NodeId, NodeId>> pointerGrid;
  Grid(CHGraph& graph, int gridSizeX, int gridSizeY):
    graph(graph), gridSizeX(gridSizeX), gridSizeY(gridSizeY) {};
  void reorderNodes();
  void buildGrid();
};
} // namespace pathFinder
#endif // MASTER_ARBEIT_GRID_H
