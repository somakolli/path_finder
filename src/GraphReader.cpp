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
  buildOffset(graph.edges.begin().base(), graph.edges.size(), graph.offset);
}

void pathFinder::GraphReader::readCHFmiFile(
    std::shared_ptr<pathFinder::CHGraph> graph, const std::string &filepath, bool reorderWithGrid) {
  std::ifstream in(filepath);
  std::string line;
  // ignore first 10 lines
  for (auto i = 0; i < 10; ++i) {
    std::getline(in, line);
  }
  in >> graph->m_numberOfNodes;
  in >> graph->m_numberOfEdges;
  graph->m_nodes = (CHNode*)std::calloc(graph->m_numberOfNodes, sizeof(CHNode));
  graph->m_edges = (CHEdge*)std::calloc(graph->m_numberOfEdges, sizeof(CHEdge));

  uint32_t type;
  uint32_t maxSpeed;
  uint32_t child1;
  uint32_t child2;
  uint64_t osmId;
  Lat lat;
  Lng lng;
  uint32_t el;
  int i = graph->m_numberOfNodes + 1;
  int j = 0;
  CHNode node{};
  while (--i > 0 &&
         in >> node.id >> osmId >> lat >> lng >> el >> node.level) {
    node.latLng = {lat, lng};
    graph->m_nodes[j++] = node;
  }
  i = graph->m_numberOfEdges + 1;
  j = 0;
  CHEdge edge{};
  while (--i > 0 && in >> edge.source >> edge.target >> edge.distance >>
                        type >> maxSpeed >> child1 >> child2) {
    child1 == -1 ? edge.child1 = std::nullopt : edge.child1 = child1;
    child2 == -1 ? edge.child2 = std::nullopt : edge.child2 = child2;
    graph->m_edges[j++] = edge;
  }
  if(reorderWithGrid){
    Grid grid;
    grid.createForGraph(*graph, 10, 10);
  }
#if TEST
  graph->randomizeLatLngs();
#endif
  graph->sortEdges();
  buildOffset(graph->m_edges, graph->m_numberOfEdges, graph->m_offset);
  buildBackEdges(graph->m_edges, graph->m_backEdges, graph->m_numberOfEdges);
  buildOffset(graph->m_backEdges, graph->m_numberOfEdges, graph->m_backOffset);
}


void pathFinder::GraphReader::sortEdges(MyIterator<CHEdge*> edges) {
  std::sort(edges.begin(), edges.end(),
            [](const auto &edge1, const auto &edge2) -> bool {
              return (edge1.source == edge2.source)
                         ? edge1.target <= edge2.target
                         : edge1.source < edge2.source;
            });
}
void pathFinder::GraphReader::buildOffset(const pathFinder::CHEdge* edges, size_t edgeSize, NodeId *&offset) {
  if (edgeSize == 0)
    return;
  NodeId numberOfNodes = edges[edgeSize - 1].source + 1;
  free(offset);
  offset = (NodeId*)std::calloc((numberOfNodes + 1), sizeof(NodeId));
  size_t offsetSize = numberOfNodes + 1;
  offset[numberOfNodes] = edgeSize;
  for (int i = edgeSize - 1; i >= 0; --i) {
    offset[edges[i].source] = i;
  }
  for (int i = edges[0].source + 1; i < offsetSize - 2; ++i) {

    if (offset[i] == 0) {
      size_t j = i + 1;
      while (offset[j] == 0) {
        ++j;
      }
      size_t offsetToSet = offset[j];
      --j;
      size_t firstNullPosition = i;
      while (j >= firstNullPosition) {
        offset[j] = offsetToSet;
        --j;
        ++i;
      }
    }
  }
  offset[0] = 0;
  offset[offsetSize] = edgeSize;
}
void pathFinder::GraphReader::buildBackEdges(const pathFinder::CHEdge *forwardEdges, pathFinder::CHEdge *&backEdges,
                                             size_t numberOfEdges) {
  free(backEdges);
  backEdges = (CHEdge*)calloc(numberOfEdges, sizeof(CHEdge));
  for (size_t i = 0; i < numberOfEdges; ++i) {
    backEdges[i].source = forwardEdges[i].target;
    backEdges[i].target = forwardEdges[i].source;
    backEdges[i].distance = forwardEdges[i].distance;
    backEdges[i].child1 = forwardEdges[i].child1;
    backEdges[i].child2 = forwardEdges[i].child2;
  }
  sortEdges(MyIterator(backEdges, backEdges + numberOfEdges));
}
void pathFinder::GraphReader::buildOffset(const pathFinder::Edge* edges, size_t edgeSize,
                                          std::vector<NodeId> &offset) {
  if (edgeSize == 0)
    return;

  NodeId numberOfNodes = edges[edgeSize - 1].source + 1;
  offset.reserve(numberOfNodes + 1);
  for(int i = 0; i < numberOfNodes + 1; ++i) {
    offset.emplace_back(0);
  }
  size_t offsetSize = numberOfNodes + 1;
  offset[numberOfNodes] = edgeSize;
  for (int i = edgeSize - 1; i >= 0; --i) {
    offset[edges[i].source] = i;
  }
  for (int i = edges[0].source + 1; i < offsetSize - 2; ++i) {

    if (offset[i] == 0) {
      size_t j = i + 1;
      while (offset[j] == 0) {
        ++j;
      }
      size_t offsetToSet = offset[j];
      --j;
      size_t firstNullPosition = i;
      while (j >= firstNullPosition) {
        offset[j] = offsetToSet;
        --j;
        ++i;
      }
    }
  }
  offset[0] = 0;
  offset[offsetSize] = edgeSize;
}
