//
// Created by sokol on 11.08.20.
//
#include <path_finder/helper/Benchmarker.h>

#include <fstream>
#include <random>
#include <utility>
#include <zconf.h>

namespace pathFinder{
[[maybe_unused]] Benchmarker::Benchmarker(const std::string &dataPath, std::string outPutPath):
    m_dataPath(dataPath), m_outputPath(std::move(outPutPath))
{
  m_pathFinderRam = FileLoader::loadHubLabelsSharedRam(dataPath);
  m_pathFinderMMap = FileLoader::loadHubLabelsShared(dataPath);
}
[[maybe_unused]] Benchmarker::Benchmarker(std::shared_ptr<HybridPathFinder>& pathFinderRam, std::shared_ptr<HybridPathFinder>& pathFinderMMap,
                         std::string  outPutPath)
                        : m_pathFinderMMap(pathFinderMMap), m_pathFinderRam(pathFinderRam), m_outputPath(std::move(outPutPath))
{}
void Benchmarker::benchmarkAllLevel(uint32_t numberOfQueries,
            std::vector<BenchResult>& ramResult,std::vector<BenchResult>& mmapResult) {
  uint32_t numberOfNodes = m_pathFinderRam->graphNodeSize();
  std::uniform_int_distribution<> distr(0, numberOfNodes-1);
  auto maxLevel = m_pathFinderRam->getMaxLevel();
  auto labelsUntilLevel = m_pathFinderRam->labelsUntilLevel();
  for(int level = maxLevel; level >= labelsUntilLevel; --level) {
    BenchResult ramResultForOneLevel;
    BenchResult mmapResultForOneLevel;
    benchmarkLevel(level, numberOfQueries, ramResultForOneLevel, mmapResultForOneLevel);
    ramResult.push_back(ramResultForOneLevel);
    mmapResult.push_back(mmapResultForOneLevel);
  }
}
void Benchmarker::benchmarkLevel(uint32_t level, uint32_t numberOfQueries,
                                 BenchResult& ramResult, BenchResult& mmapResult) {
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
      m_pathFinderMMap->setLabelsUntilLevel((Level)level);
      auto mmapResultReturn = m_pathFinderMMap->getShortestPath(sourceId, targetId);
      dropCaches();
      m_pathFinderRam->setLabelsUntilLevel((Level)level);
      auto ramResultReturn = m_pathFinderRam->getShortestPath(sourceId, targetId);
      dropCaches();
      totalRamTime += ramResultReturn.distanceTime;
      totalMMapTime += mmapResultReturn.distanceTime;
      totalCalCLabelRamTimingInfo += ramResultReturn.calcLabelTimingInfo;
      totalCalCLabelMMapTimingInfo += mmapResultReturn.calcLabelTimingInfo;
    } catch (const std::runtime_error& e) {}
  }
  double averageRamTime = totalRamTime / numberOfQueries;
  double averageMMapTime = totalMMapTime / numberOfQueries;
  totalCalCLabelRamTimingInfo /= numberOfQueries;
  totalCalCLabelMMapTimingInfo /= numberOfQueries;

  RoutingResult mmapAverageRoutingResult{};
  mmapAverageRoutingResult.calcLabelTimingInfo = totalCalCLabelMMapTimingInfo;
  mmapAverageRoutingResult.distanceTime = averageMMapTime;

  RoutingResult ramAverageRoutingResult{};
  ramAverageRoutingResult.calcLabelTimingInfo = totalCalCLabelRamTimingInfo;
  ramAverageRoutingResult.distanceTime = averageRamTime;

  mmapResult = std::make_pair(level , mmapAverageRoutingResult);
  ramResult = std::make_pair(level , ramAverageRoutingResult);

  std::string line {"--------------------"};
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
void Benchmarker::dropCaches() {
  sync();

  std::ofstream ofs("/proc/sys/vm/drop_caches");
  ofs << "3" << std::endl;
}
}
