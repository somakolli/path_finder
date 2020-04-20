//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_GRAPHREADER_H
#define ALG_ENG_PROJECT_GRAPHREADER_H

#include "CHGraph.h"
#include "Graph.h"
#include "string"

namespace pathFinder {
class GraphReader {
public:
  static void readFmiFile(Graph &graph, const std::string &filepath);
  static void readCHFmiFile(CHGraph<std::vector> &chGraph,
                            const std::string &filepath);

private:
  static void buildOffset(Graph::edgeVector &edges,
                          std::vector<NodeId> &offset);
  static void buildBackEdges(const Graph::edgeVector &forwardEdges,
                             Graph::edgeVector &backEdges);
  static void sortEdges(Graph::edgeVector &edges);
};
} // namespace pathFinder

#endif // ALG_ENG_PROJECT_GRAPHREADER_H
