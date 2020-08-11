//
// Created by sokol on 11.08.20.
//

#ifndef PATH_FINDER_BENCHMARKER_H
#define PATH_FINDER_BENCHMARKER_H

#include <memory>
#include <path_finder/storage/FileLoader.h>
#include <string>
namespace pathFinder {
class Benchmarker {
public:
  Benchmarker(const std::string& dataPath, std::string  outPutPath);
  Benchmarker(std::shared_ptr<HybridPFRam>& pathFinderRam,
              std::shared_ptr<HybridPF>& pathFinderMMap,
              std::string  outPutPath);
  void benchmarkAllLevel(uint32_t numberOfQueries);
  void benchmarkLevel(uint32_t level, uint32_t numberOfQueries);
private:
  std::string m_dataPath;
  std::string m_outputPath;
  std::shared_ptr<HybridPF> m_pathFinderMMap;
  std::shared_ptr<HybridPFRam> m_pathFinderRam;
};
}


#endif // PATH_FINDER_BENCHMARKER_H
