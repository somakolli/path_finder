//
// Created by sokol on 20.04.20.
//

#include "path_finder/graphs/Grid.h"
namespace pathFinder {
void pathFinder::Grid::buildGrid() {
  double divLat = 180.0f / static_cast<float>(gridSizeX);
  double divLng = 360.0f / static_cast<float>(gridSizeY);
  for (const auto& node: graph.getNodes()){
    auto latPositionInGrid = floor(node.latLng.lat / divLat);
    auto lngPositionInGrid = floor(node.latLng.lng / divLng);
    auto latLng = std::make_pair(latPositionInGrid, lngPositionInGrid);
    mapGrid[latLng].emplace_back(std::move(node));
  }
}
void Grid::reorderNodes() {
  std::map<NodeId, NodeId> oldIdToNewId;
  graph.getNodes();
  NodeId i = 0;
  for(auto& [latLng, nodeVector] : mapGrid) {
    auto pointerPair = std::make_pair(i, i);
    for(auto node = graph.m_nodes; node < graph.m_nodes + graph.m_numberOfNodes; ++node) {
      oldIdToNewId[node->id] = i;
      node->id = i;
      graph.m_nodes[i] = *node;
      ++i;
      pointerPair.second++;
    }
    pointerGrid[latLng] = pointerPair;
  }
  for(auto edge = graph.m_edges; edge < graph.m_edges + graph.m_numberOfEdges; ++edge) {
    edge->source = oldIdToNewId[edge->source];
    edge->target = oldIdToNewId[edge->target];
  }
  graph.sortEdges();
  free(graph.m_offset);
}
}