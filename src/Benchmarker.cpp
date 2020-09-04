//
// Created by sokol on 11.08.20.
//
#include <path_finder/helper/Benchmarker.h>


#include <path_finder/graphs/GraphStats.h>
#include <fstream>
#include <random>
#include <utility>
#include <zconf.h>

namespace pathFinder {
auto Benchmarker::benchmarkAllLevel(HybridPathFinder& pathFinder
                                    ,uint32_t numberOfQueries) -> std::vector<Benchmarker::BenchResult> {
  std::vector<BenchResult> returnVec;
  uint32_t numberOfNodes = pathFinder.graphNodeSize();
  std::uniform_int_distribution<> distr(0, numberOfNodes - 1);
  auto maxLevel = pathFinder.getMaxLevel();
  auto labelsUntilLevel = pathFinder.labelsUntilLevel();
  for (int level = maxLevel; level >= labelsUntilLevel; --level) {
    BenchResult resultForOneLevel = benchmarkLevel(pathFinder, level, numberOfQueries);
    returnVec.push_back(resultForOneLevel);
  }
  return returnVec;
}

auto Benchmarker::benchmarkLevel(HybridPathFinder& pathFinder, uint32_t level, uint32_t numberOfQueries) -> Benchmarker::BenchResult {
  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  std::uniform_int_distribution<> distr(0, pathFinder.graphNodeSize() - 1);
  RoutingResultTimingInfo totalRoutingResult{};
  for (uint32_t i = 0; i < numberOfQueries; ++i) {
    uint32_t sourceId = distr(gen);
    uint32_t targetId = distr(gen);
    // LatLng sourceLatLng(48.797818160096874,9.214439392089846);
    // LatLng targetLatLng(48.75890477584505, 9.149551391601564);
    try {
      pathFinder.setLabelsUntilLevel((Level)level);
      auto resultReturn = pathFinder.getShortestPath(sourceId, targetId).routingResultTimingInfo;
      totalRoutingResult += resultReturn;
    } catch (const std::runtime_error &e) {
    }
    dropCaches();
  }

  RoutingResultTimingInfo averageRoutingResult = totalRoutingResult / numberOfQueries;
  std::cout << "level: " << level << '\n';
  std::cout << averageRoutingResult.toJson() << '\n';
  return std::make_pair(level, averageRoutingResult);
}
void Benchmarker::dropCaches() {
  sync();

  if(system("sudo /usr/local/bin/drop_caches.sh") != 0) {
    throw std::runtime_error("could not drop caches!");
  };
}
auto Benchmarker::benchmarkCHDijkstra(CHDijkstra& chDijkstra,uint32_t numberOfQueries) -> RoutingResultTimingInfo {

  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  uint32_t numberOfNodes = chDijkstra.getNodeCount();
  std::uniform_int_distribution<> distr(0, numberOfNodes - 1);

  double totalTime = 0;
  for (int i = 0; i < numberOfQueries; ++i) {
    uint32_t sourceId = distr(gen);
    uint32_t targetId = distr(gen);
    Stopwatch mmapWatch;
    auto distance = chDijkstra.getShortestDistance(sourceId, targetId);
    auto elapsedTime = mmapWatch.elapsedMicro();
    totalTime += elapsedTime;
    dropCaches();
  }
  double averageMMapTime = totalTime / numberOfQueries;

  RoutingResultTimingInfo routingResult{};
  routingResult.distanceTime = averageMMapTime;

  std::string line = "------------------------------\n";

  std::cout << line;
  std::cout << "Normal CH" << '\n';
  std::cout << routingResult.distanceTime << '\n';
  std::cout << line;
  return routingResult;
}
auto Benchmarker::benchMarkNearestNeighbour(const CHGraph& graph,uint32_t numberOfQueries) -> double {
  double totalTime = 0;
  for (int i = 0; i < numberOfQueries; ++i) {
    Stopwatch stopwatch;
    LatLng latLng(49.52520834197442, 10.744628906250002);
    auto nodeId = graph.getNodeIdFor(latLng);
    totalTime += stopwatch.elapsedMicro();
  }
  return totalTime / numberOfQueries;
}
void Benchmarker::printRoutingResultForOctave(std::ostream &distanceStream, const std::vector<BenchResult> &ramResult) {
  std::stringstream plotLevel;
  plotLevel << "level = [";
  std::stringstream hybridRam;
  std::stringstream pathUnpackRam;
  std::stringstream searchTime;
  std::stringstream lookUpTime;
  std::stringstream mergeTime;
  hybridRam << "distance= [";
  searchTime << "searchTime = [";
  lookUpTime << "lookUpTime = [";
  mergeTime << "mergeTime = [";
  pathUnpackRam << "pathUnpackRam = [";
  bool first = true;
  for (auto [level, result] : ramResult) {
    if (!first) {
      plotLevel << ',';
      hybridRam << ',';
      pathUnpackRam << ',';
      lookUpTime << ',';
      mergeTime << ',';
      searchTime << ',';
    }
    first = false;
    plotLevel << level;
    hybridRam << result.distanceTime;
    pathUnpackRam << result.pathTime;
    mergeTime << result.calcLabelTimingInfo.mergeTime;
    searchTime << result.calcLabelTimingInfo.graphSearchTime;
    lookUpTime << result.calcLabelTimingInfo.lookUpTime;
  }
  plotLevel << "]\n";
  hybridRam << "]\n";
  pathUnpackRam << "]\n";
  lookUpTime << "]\n";
  searchTime << "]\n";
  mergeTime << "]\n";
  distanceStream << plotLevel.str();
  distanceStream << lookUpTime.str();
  distanceStream << searchTime.str();
  distanceStream << mergeTime.str();
  distanceStream << hybridRam.str();
  distanceStream << pathUnpackRam.str();
  distanceStream << "plot(level, hybridRam, \"g\")\n";
  distanceStream << "print -djpg image.jpg\n";
}
} // namespace pathFinder
