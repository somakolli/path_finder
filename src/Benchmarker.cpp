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
void Benchmarker::benchmarkAllLevel(uint32_t numberOfQueries) {
  std::random_device rd; // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  uint32_t numberOfNodes = m_pathFinderRam->graphNodeSize();
  std::uniform_int_distribution<> distr(0, numberOfNodes-1);
  auto maxLevel = m_pathFinderRam->getMaxLevel();
  auto labelsUntilLevel = m_pathFinderRam->labelsUntilLevel(); // define the range
  for(int level = maxLevel; level >= labelsUntilLevel; --level) {
    benchmarkLevel(level, numberOfQueries);
  }

}
void Benchmarker::benchmarkLevel(uint32_t level, uint32_t numberOfQueries) {
  std::random_device rd; // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  uint32_t numberOfNodes = m_pathFinderRam->graphNodeSize();
  std::uniform_int_distribution<> distr(0, numberOfNodes-1);
  double totalRamTime = 0;
  double totalMMapTime = 0;
  CalcLabelTimingInfo totalCalCLabelRamTimingInfo{};
  CalcLabelTimingInfo totalCalCLabelMMapTimingInfo{};
  for(uint32_t i = 0 ; i < numberOfQueries; ++i) {
    uint32_t sourceId = distr(gen);
    uint32_t targetId = distr(gen);
    try {
      m_pathFinderMMap->setLabelsUntilLevel(level);
      auto mmapResult = m_pathFinderMMap->getShortestPath(sourceId, targetId);
      m_pathFinderRam->setLabelsUntilLevel(level);
      auto ramResult = m_pathFinderRam->getShortestPath(sourceId, targetId);
      totalRamTime += ramResult.distanceTime;
      totalMMapTime += mmapResult.distanceTime;
      totalCalCLabelRamTimingInfo += ramResult.calcLabelTimingInfo;
      totalCalCLabelMMapTimingInfo += mmapResult.calcLabelTimingInfo;
    } catch (const std::runtime_error& error) {
      // std::cout << error.what() << '\n';
    }
  }
  double averageRamTime = totalRamTime / numberOfQueries;
  double averageMMapTime = totalMMapTime / numberOfQueries;
  totalCalCLabelRamTimingInfo /= numberOfQueries;
  totalCalCLabelMMapTimingInfo /= numberOfQueries;

  std::string line {"----------------------"};
  std::cout << "level:" << level << '\n';
  std::cout << line << '\n';
  std::cout << "ram" << '\n';
  std::cout << totalCalCLabelRamTimingInfo << '\n';
  std::cout << "totalTime: " << averageRamTime << '\n';
  std::cout << line << '\n';
  std::cout << "mmap" << '\n';
  std::cout << totalCalCLabelMMapTimingInfo << '\n';
  std::cout << "totalTime: " << averageMMapTime << '\n';
  std::cout << line << '\n';

}
}

