#ifndef CHHLBENCHMARKER_H
#define CHHLBENCHMARKER_H

#include "CHDijkstra.h"
#include "GraphStats.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace pathFinder {
template <class Graph> class ChHlBenchmarker {
private:
  Graph &graph;
  size_t numberOfQueriesPerLevel = 100;

public:
  explicit ChHlBenchmarker(Graph &graph);
  void compareSpeed(boost::filesystem::path benchFilePath, Level untilLevel, bool ram);
};
template <class Graph>
pathFinder::ChHlBenchmarker<Graph>::ChHlBenchmarker(Graph &graph)
    : graph(graph) {}

template <class Graph>
void pathFinder::ChHlBenchmarker<Graph>::compareSpeed(
    boost::filesystem::path benchFilePath, Level untilLevel, bool ram) {
  // CHDijkstra chDijkstra(graph);
  boost::filesystem::ofstream ofs{benchFilePath};
  GraphStats<Graph> gs(graph);
  boost::random::mt19937 rng;
  boost::random::uniform_int_distribution<> node(0, graph.numberOfNodes - 1);
  Level maxLevel = gs.getHighestLevel();
  ofs << "level,searchTime,mergeTime,lookUpTime,totalTime(µs),"
         "spaceConsumption\n";
  std::vector<CHNode> sortedNodes;
  graph.sortByLevel(sortedNodes);
  std::vector<Distance> cost;
  cost.reserve(graph.getNodes().size());
  while (cost.size() < graph.getNodes().size())
    cost.push_back(MAX_DISTANCE);

  Timer timer(numberOfQueriesPerLevel);
  HubLabels<HubLabelStore<std::vector>, Graph> hubLabels(
      graph, untilLevel, sortedNodes, cost, timer);
  if(!ram) {

    auto& ramHlStore = hubLabels.getHublabelStore();

    auto mmapNodes = pathFinder::MmapVector(graph.getNodes(), "nodes");
    std::cout << "test" << std::endl;
    auto mmapForwardEdges = pathFinder::MmapVector(graph.getForwardEdges(), "forwardEdges");
    auto mmapBackwardEdges = pathFinder::MmapVector(graph.getBackEdges(), "backwardEdges");
    auto diskGraph = pathFinder::CHGraph(mmapNodes, mmapForwardEdges, mmapBackwardEdges, graph.getForwardOffset(), graph.getBackOffset(), graph.numberOfNodes);
    auto mmapForwardLabels = pathFinder::MmapVector(ramHlStore.getForwardLabels(), "forwardLabels");
    auto mmapBackwardLabels = pathFinder::MmapVector(ramHlStore.getBackwardLabels(), "backwardLabels");
    pathFinder::HubLabelStore diskHlStore(mmapForwardLabels, mmapBackwardLabels, ramHlStore.getForwardOffset(), ramHlStore.getBackwardOffset());
    pathFinder::HubLabels diskHl(diskGraph, untilLevel, diskHlStore, timer);
    ramHlStore.getBackwardLabels().clear();
    ramHlStore.getBackwardLabels().shrink_to_fit();
    ramHlStore.getForwardLabels().clear();
    ramHlStore.getForwardLabels().shrink_to_fit();
    graph.deleteEdges();
    graph.deleteNodes();

    for (int i = maxLevel + 1; i >= untilLevel; --i) {
      timer.resetAll();
      diskHl.setLabelsUntilLevel(i);
      for (int j = 0; j < numberOfQueriesPerLevel; ++j) {
        Stopwatch sw;
        diskHl.getShortestDistance(node(rng), node(rng)).value();
        timer.addTotalTime(
            std::chrono::duration_cast<std::chrono::microseconds>(sw.elapsed())
                .count());
      }
      ofs << i << "," << timer.getAverageSearchTime() << ","
          << timer.getAverageMergeTime() << "," << timer.getAverageLookUpTime()
          << "," << timer.getAverageTotalTime() << ","
          <<hubLabels.spaceMeasurer.getSpaceConsumption(i) << std::endl;
      std::cout << "finished level: " << i << std::endl;
    }
  } else {
    for (int i = maxLevel + 1; i >= untilLevel; --i) {
      timer.resetAll();
      hubLabels.setLabelsUntilLevel(i);
      for (int j = 0; j < numberOfQueriesPerLevel; ++j) {
        Stopwatch sw;
        hubLabels.getShortestDistance(node(rng), node(rng)).value();
        timer.addTotalTime(
            std::chrono::duration_cast<std::chrono::microseconds>(sw.elapsed())
                .count());
      }
      ofs << i << "," << timer.getAverageSearchTime() << ","
          << timer.getAverageMergeTime() << "," << timer.getAverageLookUpTime()
          << "," << timer.getAverageTotalTime() << ","
          << hubLabels.spaceMeasurer.getSpaceConsumption(i) << std::endl;
      std::cout << "finished level: " << i << std::endl;
    }
  }
}
} // namespace pathFinder

#endif