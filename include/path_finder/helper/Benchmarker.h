#pragma once
#include <path_finder/routing/CHDijkstra.h>
#include <path_finder/storage/FileLoader.h>
#include <string>
namespace pathFinder {
class Benchmarker {
public:
  using BenchResult = std::pair<Level, RoutingResultTimingInfo>;
  Benchmarker(const std::string &dataPath, std::string outPutPath);
  Benchmarker(std::shared_ptr<HybridPathFinder> pathFinderRam, std::shared_ptr<HybridPathFinder> pathFinderMMap,
              std::shared_ptr<CHDijkstra> chDijkstraRam, std::shared_ptr<CHDijkstra> chDijkstraMMap,
              std::string outPutPath);
  std::vector<BenchResult> benchmarkAllLevel(uint32_t numberOfQueries);
  BenchResult benchmarkLevel(uint32_t level, uint32_t numberOfQueries);
  double benchMarkNearestNeighbour(uint32_t numberOfQueries);
  RoutingResultTimingInfo benchmarkCHDijkstra(uint32_t numberOfQueries);
  static void printRoutingResultForOctave(std::ostream &distanceStream, const std::vector<BenchResult> &ramResult,
                                          double chResult) {
    std::stringstream plotLevel;
    plotLevel << "level = [";
    std::stringstream hybridRam;
    std::stringstream chRam;
    std::stringstream pathUnpackRam;
    hybridRam << "hybridRam = [";
    chRam << "chRam = [";
    pathUnpackRam << "pathUnpackRam = [";
    bool first = true;
    for (auto [level, result] : ramResult) {
      if (!first) {
        plotLevel << ',';
        hybridRam << ',';
        chRam << ',';
        pathUnpackRam << ',';
      }
      first = false;
      plotLevel << level;
      hybridRam << result.distanceTime;
      pathUnpackRam << result.pathTime;
    }
    plotLevel << "]\n";
    hybridRam << "]\n";
    chRam << "]\n";
    pathUnpackRam << "]\n";
    distanceStream << plotLevel.str();
    distanceStream << hybridRam.str();
    distanceStream << chRam.str();
    distanceStream << "plot(level, hybridRam, \"g\", level, chRam, \"g\")\n";
    distanceStream << "print -djpg image.jpg\n";
    distanceStream << pathUnpackRam.str();
  }

private:
  std::string m_dataPath;
  std::string m_outputPath;
  std::shared_ptr<HybridPathFinder> m_pathFinder;
  std::shared_ptr<CHDijkstra> m_chDijkstra;
  static void dropCaches();
};
} // namespace pathFinder
