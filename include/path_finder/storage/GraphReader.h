#pragma once
#include "path_finder/graphs/CHGraph.h"

#include <path_finder/graphs/Grid.h>
#include <string>

namespace pathFinder {
class GraphReader {
public:
  static void readFmiFile(Graph &graph, const std::string &filepath);
  static void readCHFmiFile(std::shared_ptr<pathFinder::CHGraph> chGraph, const std::string &filepath,
                            bool reorderWithGrid);
  static void buildOffset(const CHEdge *edges, size_t edgeSize, NodeId *&offset);
  static void buildOffset(const Edge *edges, size_t edgeSize, std::vector<NodeId> &offset);

private:
  static void buildBackEdges(const CHEdge *forwardEdges, CHEdge *&backEdges, size_t numberOfEdges);
  static void sortEdges(MyIterator<CHEdge *> edges);
  static void createGridForGraph(CHGraph &graph, double latStretchFactor, double lngStretchFactor);
  static void calcBoundingBox(CHGraph &graph);
};
} // namespace pathFinder
