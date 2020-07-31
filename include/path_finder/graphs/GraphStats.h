//
// Created by sokol on 24.11.19.
//

#ifndef ALG_ENG_PROJECT_GRAPHSTATS_H
#define ALG_ENG_PROJECT_GRAPHSTATS_H

#include "path_finder/graphs/CHGraph.h"

namespace pathFinder {
template <typename Graph> class GraphStats {
  Graph &graph;

public:
  explicit GraphStats(Graph &graph);
  std::uint32_t getDegree(NodeId id);
  float getAverageDegree();
  std::uint32_t getNodeCountWithLevelSmallerThan(Level level);
  Level getHighestLevel();
  void printStats(std::ostream &ss);
};
template <typename Graph>
pathFinder::GraphStats<Graph>::GraphStats(Graph &graph) : graph(graph) {}
template <typename Graph>
std::uint32_t pathFinder::GraphStats<Graph>::getDegree(pathFinder::NodeId id) {
  return graph.edgesFor(id, EdgeDirection::BACKWARD).size() +
         graph.edgesFor(id, EdgeDirection::FORWARD).size();
}
template <typename Graph>
float pathFinder::GraphStats<Graph>::getAverageDegree() {
  size_t accumulator = 0;
  for (auto node : graph.getNodes()) {
    accumulator += getDegree(node.id);
  }
  return (float)accumulator / graph.numberOfNodes;
}
template <typename Graph>
std::uint32_t pathFinder::GraphStats<Graph>::getNodeCountWithLevelSmallerThan(
    pathFinder::Level level) {
  size_t count = 0;
  for (auto node : graph.getNodes()) {
    if (node.level < level)
      ++count;
  }
  return count;
}
template <typename Graph>
pathFinder::Level pathFinder::GraphStats<Graph>::getHighestLevel() {
  Level highestLevel = 0;
  for (const auto &node : graph.getNodes())
    if (node.level > highestLevel)
      highestLevel = node.level;
  return highestLevel;
}
template <typename Graph>
void pathFinder::GraphStats<Graph>::printStats(std::ostream &ss) {
  ss << "total node size: " << graph.numberOfNodes << "\n";
  ss << "total edge size: " << graph.edges.size() << "\n";
  ss << "average degree: " << getAverageDegree() << "\n";
  Level highestLevel = getHighestLevel();
  ss << "highest level: " << highestLevel << "\n";
  std::string seperator =
      "------------------------------------------------------";
  ss << seperator << "\n";
  ss << "number of nodes < level\n";
  for (Level level = 1; level < 50; ++level) {
    size_t nodeCount = getNodeCountWithLevelSmallerThan(level);
    ss << nodeCount << ',' << level << ','
       << (float)nodeCount / graph.numberOfNodes << '\n';
  }
}
} // namespace pathFinder
#endif // ALG_ENG_PROJECT_GRAPHSTATS_H