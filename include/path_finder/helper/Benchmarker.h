//
// Created by sokol on 11.08.20.
//

#ifndef PATH_FINDER_BENCHMARKER_H
#define PATH_FINDER_BENCHMARKER_H

#include <path_finder/routing/CHDijkstra.h>
#include <path_finder/storage/FileLoader.h>
#include <string>
namespace pathFinder {
class Benchmarker {
public:
  using BenchResult = std::pair<Level, RoutingResult>;
  Benchmarker(const std::string& dataPath, std::string  outPutPath);
  Benchmarker(std::shared_ptr<HybridPathFinder> pathFinderRam,
              std::shared_ptr<HybridPathFinder> pathFinderMMap,
              std::shared_ptr<CHDijkstra> chDijkstraRam,
              std::shared_ptr<CHDijkstra> chDijkstraMMap,
              std::string  outPutPath);
  void benchmarkAllLevel(uint32_t numberOfQueries, std::vector<BenchResult>& ramResult,std::vector<BenchResult>& mmapResult);
  void benchmarkLevel(uint32_t level, uint32_t numberOfQueries, BenchResult& ramResult, BenchResult& mmapResult);
  void benchmarkCHDijkstra(uint32_t numberOfLevels, RoutingResult& chMMapResult, RoutingResult& chRamResult);
  static void printRoutingResultForOktave(std::ostream& os,const std::vector<BenchResult>& ramResult,
                                          const std::vector<BenchResult>& mmapResult, double chRamResult, double chMMapResult) {
    std::stringstream plotLevel;
    plotLevel << "level = [";
    std::stringstream hybridRam;
    std::stringstream chRam;
    hybridRam << "hybridRam = [";
    hybridRam << "chRam = [";
    bool first = true;
    for(auto [level, result]:ramResult) {
      if(!first){
        plotLevel << ',';
        hybridRam << ',';
        chRam << ',';
      }
      first = false;
      plotLevel << level;
      hybridRam << result.distanceTime;
      chRam << chRamResult;
    }
    std::stringstream hybridMmap;
    std::stringstream chMMap;
    hybridMmap << "hybridMMap = [";
    chMMap << "chMMap = [";
    first = true;
    for(auto [level, result]: mmapResult) {
      if(!first){
        hybridMmap << ',';
        chMMap << ',';
      }
      first = false;
      hybridMmap << result.distanceTime;
      chMMap << chMMapResult;
    }
    plotLevel << "]\n";
    hybridRam << "]\n";
    hybridMmap << "]\n";
    chRam << "]\n";
    chMMap << "]\n";
    os << plotLevel.str();
    os << hybridRam.str();
    os << hybridMmap.str();
    os << chRam.str();
    os << chMMap.str();
    os << "plot(level, hybridRam, \"g\",level, hybridMMap, \"r\", level, chRam, \"g\", level, chMMap, \"r\")\n";
    os << "print -djpg image.jpg\n";
  }
private:
  std::string m_dataPath;
  std::string m_outputPath;
  std::shared_ptr<HybridPathFinder> m_pathFinderMMap;
  std::shared_ptr<HybridPathFinder> m_pathFinderRam;
  std::shared_ptr<CHDijkstra> m_chDijkstraMMap;
  std::shared_ptr<CHDijkstra> m_chDijkstraRam;
  static void dropCaches();
};
}


#endif // PATH_FINDER_BENCHMARKER_H
