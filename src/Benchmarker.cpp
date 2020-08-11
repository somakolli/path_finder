//
// Created by sokol on 11.08.20.
//
#include <path_finder/helper/Benchmarker.h>

#include <random>
#include <utility>

namespace pathFinder{
[[maybe_unused]] Benchmarker::Benchmarker(const std::string &dataPath, std::string outPutPath):
    m_dataPath(dataPath), m_outputPath(std::move(outPutPath))
{
  m_pathFinderRam = FileLoader::loadHubLabelsSharedRam(dataPath);
  m_pathFinderMMap = FileLoader::loadHubLabelsShared(dataPath);
}
[[maybe_unused]] Benchmarker::Benchmarker(std::shared_ptr<HybridPFRam>& pathFinderRam, std::shared_ptr<HybridPF>& pathFinderMMap,
                         std::string  outPutPath)
                        : m_pathFinderMMap(pathFinderMMap), m_pathFinderRam(pathFinderRam), m_outputPath(std::move(outPutPath))
{}
void Benchmarker::runPathBenchmark(uint32_t numberOfQueries) {
  std::random_device rd; // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  uint32_t numberOfNodes = m_pathFinderRam->graphNodeSize();
  std::uniform_int_distribution<> distr(0, numberOfNodes-1); // define the range
for(int level = m_pathFinderRam->getMaxLevel(); level >= 0; --level) {
  for(uint32_t i = 0 ; i < numberOfQueries; ++i) {
    uint32_t sourceId = distr(gen);
    uint32_t targetId = distr(gen);

    auto ramResult = m_pathFinderRam->getShortestPath(sourceId, targetId);
    auto mmapResult = m_pathFinderMMap->getShortestPath(sourceId, targetId);
    std::cout << "ramDistanceTime:"<< ramResult.distanceTime << '\n';
    std::cout << "mmapDistanceTime:"<< mmapResult.distanceTime << '\n';
  }
}

}
}

