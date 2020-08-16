//
// Created by sokol on 11.08.20.
//

#ifndef PATH_FINDER_BENCHMARKER_H
#define PATH_FINDER_BENCHMARKER_H

#include <path_finder/storage/FileLoader.h>
#include <string>
namespace pathFinder {
class Benchmarker {
public:
  using BenchResult = std::pair<Level, RoutingResult>;
  Benchmarker(const std::string& dataPath, std::string  outPutPath);
  Benchmarker(std::shared_ptr<HybridPFRam>& pathFinderRam,
              std::shared_ptr<HybridPF>& pathFinderMMap,
              std::string  outPutPath);
  void benchmarkAllLevel(uint32_t numberOfQueries, std::vector<BenchResult>& ramResult,std::vector<BenchResult>& mmapResult);
  void benchmarkLevel(uint32_t level, uint32_t numberOfQueries, BenchResult& ramResult, BenchResult& mmapResult);
  static void printRoutingResultForOktave(std::ostream& os,const std::vector<BenchResult>& ramResult,
                                          const std::vector<BenchResult>& mmapResult) {
    std::stringstream x;
    x << "x = [";
    std::stringstream y0;
    y0 << "y0 = [";
    bool first = true;
    for(auto [level, result]:ramResult) {
      if(!first){
        x << ',';
        y0 << ',';
      }
      first = false;
      x << level;
      y0 << result.distanceTime;
    }
    std::stringstream y1;
    y1 << "y1 = [";
    first = true;
    for(auto [level, result]: mmapResult) {
      if(!first)
        y1 << ',';
      first = false;
      y1 << result.distanceTime;
    }
    x << "]\n";
    y0 << "]\n";
    y1 << "]\n";
    os << x.str();
    os << y0.str();
    os << y1.str();
    os << "plot(x, y0, \"g\",x, y1, \"r\")\n";
    os << "print -djpg image.jpg\n";
  }
private:
  std::string m_dataPath;
  std::string m_outputPath;
  std::shared_ptr<HybridPF> m_pathFinderMMap;
  std::shared_ptr<HybridPFRam> m_pathFinderRam;
  static void dropCaches();
};
}


#endif // PATH_FINDER_BENCHMARKER_H
