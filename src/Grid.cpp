//
// Created by sokol on 20.04.20.
//
#include <path_finder/graphs/CHGraph.h>
#include <path_finder/graphs/Grid.h>

void pathFinder::Grid::createForGraph(pathFinder::CHGraph &graph, double latStretchFactor, double lngStretchFactor) {
  std::map<std::pair<int, int>, std::vector<CHNode>> map;

  for(const auto& node : graph.getNodes()) {
    int latPositionInGrid = std::floor(node.latLng.lat * latStretchFactor);
    int lngPositionInGrid = std::floor(node.latLng.lng * lngStretchFactor);

    map[std::make_pair(latPositionInGrid, lngPositionInGrid)].emplace_back(node);
  }
  std::map<NodeId, NodeId> oldIdToNewId;
  size_t i = 0;
  free(graph.m_nodes);
  graph.m_nodes = (CHNode*) std::calloc(graph.m_numberOfNodes, sizeof(CHNode));
  for(auto& [positionPair, nodeVec] : map) {
    NodeId begin = i;
    for(const auto& node : nodeVec) {
      oldIdToNewId[node.id] = i;
      CHNode newNode = node;
      newNode.id = i;
      graph.m_nodes[i] = newNode;
      ++i;
      if(i > graph.m_numberOfNodes)
        throw std::out_of_range("grid has more nodes that graph");
    }
    NodeId end = i;
    m_grid[positionPair] = std::make_pair(begin, end);
  }
  size_t j = 0;
  for(auto& edge : graph.getEdgesMutable()) {
    edge.source = oldIdToNewId[edge.source];
    edge.target = oldIdToNewId[edge.target];
    ++j;
    if(j > graph.m_numberOfEdges)
      throw std::out_of_range("");
  }
  graph.gridMap = m_grid;
}
auto pathFinder::Grid::begin() {
  return m_grid.cbegin();
}
auto pathFinder::Grid::end() {
  return m_grid.cend();
}
