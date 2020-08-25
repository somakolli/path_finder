//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_GRAPHREADER_H
#define ALG_ENG_PROJECT_GRAPHREADER_H

#include "path_finder/graphs/CHGraph.h"

#include <string>

namespace pathFinder {
class GraphReader {
public:
  static void readFmiFile(Graph &graph, const std::string &filepath);
  static void readCHFmiFile(std::shared_ptr<pathFinder::CHGraph> chGraph, const std::string &filepath, bool reorderWithGrid);
  static NodeId* buildOffset(const CHEdge* edges, size_t edgeSize);
  static void buildOffset(const Edge* edges, size_t edgeSize, std::vector<NodeId>& offset);
private:
  static void buildBackEdges(const CHEdge* forwardEdges, CHEdge* &backEdges, size_t numberOfEdges);
  static void sortEdges(MyIterator<CHEdge*> edges);
};
} // namespace pathFinder

#endif // ALG_ENG_PROJECT_GRAPHREADER_H
