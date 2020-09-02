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
[[maybe_unused]] Benchmarker::Benchmarker(const std::string &dataPath, std::string outPutPath)
    : m_dataPath(dataPath), m_outputPath(std::move(outPutPath)) {
  m_pathFinder = FileLoader::loadHubLabelsShared(dataPath);
  m_chDijkstra = FileLoader::loadCHDijkstraShared(dataPath);
}
[[maybe_unused]] Benchmarker::Benchmarker(std::shared_ptr<HybridPathFinder> pathFinderRam,
                                          std::shared_ptr<HybridPathFinder> pathFinderMMap,
                                          std::shared_ptr<CHDijkstra> chDijkstraRam,
                                          std::shared_ptr<CHDijkstra> chDijkstraMMap, std::string outPutPath)
    : m_pathFinder(pathFinderMMap), m_outputPath(std::move(outPutPath)) {}
std::vector<Benchmarker::BenchResult> Benchmarker::benchmarkAllLevel(uint32_t numberOfQueries) {
  std::vector<BenchResult> returnVec;
  uint32_t numberOfNodes = m_pathFinder->graphNodeSize();
  std::uniform_int_distribution<> distr(0, numberOfNodes - 1);
  auto maxLevel = m_pathFinder->getMaxLevel();
  auto labelsUntilLevel = m_pathFinder->labelsUntilLevel();
  for (int level = maxLevel; level >= labelsUntilLevel; --level) {
    BenchResult resultForOneLevel = benchmarkLevel(level, numberOfQueries);
    returnVec.push_back(resultForOneLevel);
  }
  return returnVec;
}

Benchmarker::BenchResult Benchmarker::benchmarkLevel(uint32_t level, uint32_t numberOfQueries) {
  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  std::uniform_int_distribution<> distr(0, m_pathFinder->graphNodeSize() - 1);
  RoutingResultTimingInfo totalRoutingResult{};
  for (uint32_t i = 0; i < numberOfQueries; ++i) {
    uint32_t sourceId = distr(gen);
    uint32_t targetId = distr(gen);
    // LatLng sourceLatLng(48.797818160096874,9.214439392089846);
    // LatLng targetLatLng(48.75890477584505, 9.149551391601564);
    try {
      m_pathFinder->setLabelsUntilLevel((Level)level);
      auto resultReturn = m_pathFinder->getShortestPath(sourceId, targetId).routingResultTimingInfo;
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

  std::ofstream ofs("/proc/sys/vm/drop_caches");
  ofs << "3" << std::endl;
}
RoutingResultTimingInfo Benchmarker::benchmarkCHDijkstra(uint32_t numberOfQueries) {

  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  uint32_t numberOfNodes = m_pathFinder->graphNodeSize();
  std::uniform_int_distribution<> distr(0, numberOfNodes - 1);

  double totalTime = 0;
  for (int i = 0; i < numberOfQueries; ++i) {
    uint32_t sourceId = distr(gen);
    uint32_t targetId = distr(gen);
    Stopwatch mmapWatch;
    auto distance = m_chDijkstra->getShortestDistance(sourceId, targetId);
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
double Benchmarker::benchMarkNearestNeighbour(uint32_t numberOfQueries) {
  double totalTime = 0;
  for (int i = 0; i < numberOfQueries; ++i) {
    Stopwatch stopwatch;
    LatLng latLng(49.52520834197442, 10.744628906250002);
    auto nodeId = m_pathFinder->getGraph()->getNodeIdFor(latLng);
    totalTime += stopwatch.elapsedMicro();
    CHNode node = m_pathFinder->getGraph()->getNode(nodeId);
  }
  return totalTime / numberOfQueries;
}
} // namespace pathFinder
