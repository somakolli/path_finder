//
// Created by sokol on 20.04.20.
//

#ifndef MASTER_ARBEIT_GRID_H
#define MASTER_ARBEIT_GRID_H
#include "CHGraph.h"
#include "Graph.h"
#include "GraphReader.h"
#include <cmath>
#include <map>
#include <utility>
namespace pathFinder {
template<typename GridNode = CHNode, typename Graph = CHGraph<std::vector>>
class Grid {
  using GridKey = std::pair<Lat, Lng>;
  std::vector<GridNode> nodes;
  int gridSizeX;
  int gridSizeY;

public:
  std::map<GridKey, std::vector<GridNode>> grid;
  std::map<GridKey, std::pair<NodeId, NodeId>> pointerGrid;
  Grid(const std::vector<GridNode> nodes, int gridSizeX, int gridSizeY):
    nodes(nodes), gridSizeX(gridSizeX), gridSizeY(gridSizeY) {};
  void reorderNodes(Graph& graph);
  void buildGrid();
};
template <typename GridNode, typename Graph>
void Grid<GridNode, Graph>::buildGrid() {
  double divLat = 180.0f / static_cast<float>(gridSizeX);
  double divLng = 360.0f / static_cast<float>(gridSizeY);
  for (const auto& node: nodes){
    auto latPositionInGrid = floor(node.latLng.lat / divLat);
    auto lngPositionInGrid = floor(node.latLng.lng / divLng);
    auto latLng = std::make_pair(latPositionInGrid, lngPositionInGrid);
    grid[latLng].emplace_back(std::move(node));
  }
}
template <typename GridNode, typename Graph>
void Grid<GridNode, Graph>::reorderNodes(Graph &graph) {
  std::map<NodeId, NodeId> oldIdToNewId;
  graph.getNodes().clear();
  NodeId i = 0;
  for(auto& [latLng, nodeVector] : grid) {
    auto pointerPair = std::make_pair(i, i);
    for(auto& node: nodeVector) {
      oldIdToNewId[node.id] = i;
      node.id = i;
      graph.getNodes().emplace_back(node);
      ++i;
      pointerPair.second++;
    }
    pointerGrid[latLng] = pointerPair;
  }
  for(auto& edge: graph.m_edges) {
    edge.source = oldIdToNewId[edge.source];
    edge.target = oldIdToNewId[edge.target];
  }
  graph.sortEdges();
  graph.m_offset.clear();
  GraphReader::buildOffset(graph.m_edges, graph.m_offset);
}
} // namespace pathFinder
#endif // MASTER_ARBEIT_GRID_H
