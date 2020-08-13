//
// Created by sokol on 02.10.19.
//
#include "path_finder/storage/GraphReader.h"
#include "path_finder/graphs/Grid.h"
#include <boost/algorithm/string.hpp>
#include <fcntl.h>
#include <fstream>
#include <iostream>

void pathFinder::GraphReader::readFmiFile(pathFinder::Graph &graph,
                                          const std::string &filepath) {
  uint32_t numberOfEdges{};
  std::ifstream in(filepath);
  std::string line;
  // ignore first 5 lines
  for (auto i = 0; i < 5; ++i) {
    std::getline(in, line);
  }
  in >> graph.numberOfNodes;
  in >> numberOfEdges;
  uint32_t type;
  uint32_t maxSpeed;
  int i = graph.numberOfNodes + 1;
  Node node{};
  long id2;
  double elevation;
  while (--i > 0 && in >> node.id >> id2 >> node.latLng.lat >>
                        node.latLng.lng >> elevation) {
    graph.nodes.push_back(node);
  }
  i = numberOfEdges + 1;
  Edge edge{};
  while (--i > 0 && in >> edge.source >> edge.target >> edge.distance >>
                        type >> maxSpeed) {
    graph.edges.push_back(edge);
  }
  buildOffset(graph.edges, graph.offset);
}

void pathFinder::GraphReader::readCHFmiFile(
    pathFinder::CHGraph<std::vector> &graph, const std::string &filepath, bool reorderWithGrid) {
  uint32_t numberOfEdges{};
  std::ifstream in(filepath);
  std::string line;
  // ignore first 10 lines
  for (auto i = 0; i < 10; ++i) {
    std::getline(in, line);
  }
  in >> graph.m_numberOfNodes;
  in >> numberOfEdges;
  uint32_t type;
  uint32_t maxSpeed;
  uint32_t child1;
  uint32_t child2;
  uint64_t osmId;
  Lat lat;
  Lng lng;
  uint32_t el;
  int i = graph.m_numberOfNodes + 1;
  CHNode node{};
  while (--i > 0 &&
         in >> node.id >> osmId >> lat >> lng >> el >> node.level) {
    node.latLng = {lat, lng};
    graph.getNodes().push_back(node);
  }
  i = numberOfEdges + 1;
  CHEdge edge{};
  while (--i > 0 && in >> edge.source >> edge.target >> edge.distance >>
                        type >> maxSpeed >> child1 >> child2) {
    child1 == -1 ? edge.child1 = std::nullopt : edge.child1 = child1;
    child2 == -1 ? edge.child2 = std::nullopt : edge.child2 = child2;
    graph.m_edges.push_back(edge);
  }

#if TEST
  graph.randomizeLatLngs();
#endif
  if(reorderWithGrid)
    gridReorder(graph);
  buildOffset(graph.m_edges, graph.m_offset);
  buildBackEdges(graph.m_edges, graph.getBackEdges());
  buildOffset(graph.getBackEdges(), graph.getBackOffset());

}

void pathFinder::GraphReader::buildBackEdges(
    const std::vector<CHEdge> &forwardEdges, std::vector<CHEdge> &backEdges) {
  for (const auto &edge : forwardEdges) {
    auto backWardEdge = edge;
    backWardEdge.source = edge.target;
    backWardEdge.target = edge.source;
    backEdges.push_back(backWardEdge);
  }
  sortEdges(backEdges);
}

void pathFinder::GraphReader::sortEdges(std::vector<CHEdge> &edges) {
  std::sort(edges.begin(), edges.end(),
            [](const auto &edge1, const auto &edge2) -> bool {
              return (edge1.source == edge2.source)
                         ? edge1.target <= edge2.target
                         : edge1.source < edge2.source;
            });
}
void pathFinder::GraphReader::gridReorder(pathFinder::CHGraph<std::vector>& graph) {
  Grid grid(graph, 3600, 1800);
  grid.buildGrid();
  grid.reorderNodes();
  std::cout << "pointer grid length " << grid.pointerGrid.size() << std::endl;
  graph.gridMap = grid.pointerGrid;
}
