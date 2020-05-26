#ifndef CHHLBENCHMARKER_H
#define CHHLBENCHMARKER_H

#include "CHDijkstra.h"
#include "GraphStats.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <fcntl.h>

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
  // first timer is for ram second timer is for disk
  std::map<int, std::pair<Timer, Timer>> timings;

  for (int i = maxLevel + 1; i >= untilLevel; --i) {
    timings[i] = std::make_pair(Timer(), Timer());
    timer = Timer(numberOfQueriesPerLevel);
    hubLabels.setTimer(timer);
    hubLabels.setLabelsUntilLevel(i);
    for (int j = 0; j < numberOfQueriesPerLevel; ++j) {
      Stopwatch sw;
      hubLabels.getShortestDistance(node(rng), node(rng)).value();
      timer.addTotalTime(
          std::chrono::duration_cast<std::chrono::microseconds>(sw.elapsed())
              .count());
    }
    timings[i].first = timer;
    std::cout << "finished ram level: " << i << std::endl;
  }

  auto& ramHlStore = hubLabels.getHublabelStore();

  auto mmapNodes = pathFinder::MmapVector(graph.getNodes(), "nodes");
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
    int fd = ::open("/proc/sys/vm/drop_caches", O_WRONLY);
    if (2 != ::write(fd, "1\n", 2)) {
      //throw std::runtime_error("Benchmarker: could not drop caches");
    }
    timer = Timer(numberOfQueriesPerLevel);
    diskHl.setTimer(timer);
    diskHl.setLabelsUntilLevel(i);
    for (int j = 0; j < numberOfQueriesPerLevel; ++j) {
      Stopwatch sw;
      diskHl.getShortestDistance(node(rng), node(rng)).value();
      timer.addTotalTime(
          std::chrono::duration_cast<std::chrono::microseconds>(sw.elapsed())
              .count());
    }
    timings[i].second = timer;
    std::cout << "finished disk level: " << i << std::endl;
  }

  for(auto&& [level, timers] : timings) {
    timer = timers.first;
    ofs << "ram,"
        << level << ","
        << timer.getAverageSearchTime() << ","
        << timer.getAverageMergeTime() << ","
        << timer.getAverageNumbersOfLabels() << ","
        << timer.getAverageItemNumber() << ","
        << timer.getAverageLookUpTime()<< ","
        << timer.getAverageTotalTime() << ","
        << hubLabels.spaceMeasurer.getSpaceConsumption(level)
        << std::endl;
    timer = timers.second;
    ofs << "disk,"
        << level << ","
        << timer.getAverageSearchTime() << ","
        << timer.getAverageMergeTime() << ","
        << timer.getAverageNumbersOfLabels() << ","
        << timer.getAverageItemNumber() << ","
        << timer.getAverageLookUpTime()<< ","
        << timer.getAverageTotalTime() << ","
        << hubLabels.spaceMeasurer.getSpaceConsumption(level)
        << std::endl;
  }
}
} // namespace pathFinder

#endif