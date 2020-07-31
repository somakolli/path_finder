//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_GRAPHREADER_H
#define ALG_ENG_PROJECT_GRAPHREADER_H

#include "path_finder/graphs/CHGraph.h"
#include "path_finder/graphs/Graph.h"
#include "string"

namespace pathFinder {
class GraphReader {
public:
  static void readFmiFile(Graph &graph, const std::string &filepath);
  static void readCHFmiFile(pathFinder::CHGraph<std::vector> &chGraph,
                            const std::string &filepath, bool reorderWithGrid);
  template <typename EdgeVector>
  static void buildOffset(EdgeVector &edges,
                          std::vector<NodeId> &offset);
private:
  static void buildBackEdges(const std::vector<CHEdge> &forwardEdges,
                             std::vector<CHEdge> &backEdges);
  static void sortEdges(std::vector<CHEdge> &edges);
  static void gridReorder(CHGraph<std::vector>& graph);
};

template <typename EdgeVector>
void pathFinder::GraphReader::buildOffset(EdgeVector &edges,
                                          std::vector<NodeId> &offset) {
  offset.clear();
  if (edges.empty())
    return;
  NodeId numberOfNodes = edges[edges.size() - 1].source + 1;
  offset.reserve(numberOfNodes + 1);
  while (offset.size() <= numberOfNodes) {
    offset.emplace_back(0);
  }
  offset[numberOfNodes] = edges.size();
  for (int i = edges.size() - 1; i >= 0; --i) {
    offset[edges[i].source] = i;
  }
  for (int i = edges[0].source + 1; i < offset.size() - 2; ++i) {

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
  offset[offset.size() - 1] = edges.size();
}
} // namespace pathFinder

#endif // ALG_ENG_PROJECT_GRAPHREADER_H
