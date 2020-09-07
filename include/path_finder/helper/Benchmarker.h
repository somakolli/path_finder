#pragma once
#include <path_finder/routing/CHDijkstra.h>
#include <path_finder/storage/FileLoader.h>
#include <string>
namespace pathFinder {
class Benchmarker {
public:
  using BenchResult = std::pair<Level, RoutingResultTimingInfo>;
  static auto benchmarkAllLevel(HybridPathFinder& pathFinder,
                         uint32_t numberOfQueries) -> std::vector<BenchResult>;
  static auto benchmarkLevel(HybridPathFinder& pathFinder, uint32_t level, uint32_t numberOfQueries) -> BenchResult;
  static auto benchMarkNearestNeighbour(const CHGraph& chGraph, uint32_t numberOfQueries) -> double;
  static auto benchmarkCHDijkstra(CHDijkstra& chDijkstra, uint32_t numberOfQueries) -> RoutingResultTimingInfo;
  static void printRoutingResultForOctave(std::ostream &distanceStream, const std::vector<BenchResult> &ramResult);

private:
  static void dropCaches();
};
} // namespace pathFinder
