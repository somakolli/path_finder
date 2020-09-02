#pragma once
#include "path_finder/graphs/CHGraph.h"

namespace pathFinder {
template <typename Graph>
class GraphStats {
  Graph &graph;

public:
  explicit GraphStats(Graph &graph);
  auto getDegree(NodeId id) -> std::uint32_t;
  auto getAverageDegree() -> float;
  auto getNodeCountWithLevelSmallerThan(Level level) -> std::uint32_t;
  auto getHighestLevel() -> Level;
  void printStats(std::ostream &ss);
};
template <typename Graph>
pathFinder::GraphStats<Graph>::GraphStats(Graph &graph) : graph(graph) {}
template <typename Graph>
auto pathFinder::GraphStats<Graph>::getDegree(pathFinder::NodeId id) -> std::uint32_t {
  return graph.edgesFor(id, EdgeDirection::BACKWARD).size() + graph.edgesFor(id, EdgeDirection::FORWARD).size();
}
template <typename Graph>
auto pathFinder::GraphStats<Graph>::getAverageDegree() -> float {
  size_t accumulator = 0;
  for (auto node : graph.getNodes()) {
    accumulator += getDegree(node.id);
  }
  return (float)accumulator / graph.numberOfNodes;
}
template <typename Graph>
auto pathFinder::GraphStats<Graph>::getNodeCountWithLevelSmallerThan(pathFinder::Level level) -> std::uint32_t {
  size_t count = 0;
  for (auto node : graph.getNodes()) {
    if (node.level < level)
      ++count;
  }
  return count;
}
template <typename Graph>
auto pathFinder::GraphStats<Graph>::getHighestLevel() -> pathFinder::Level {
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
  std::string seperator = "------------------------------------------------------";
  ss << seperator << "\n";
  ss << "number of nodes < level\n";
  for (Level level = 1; level < 50; ++level) {
    size_t nodeCount = getNodeCountWithLevelSmallerThan(level);
    ss << nodeCount << ',' << level << ',' << (float)nodeCount / graph.numberOfNodes << '\n';
  }
}
} // namespace pathFinder
