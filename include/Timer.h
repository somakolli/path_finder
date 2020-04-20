#include "Graph.h"
#include <cstdint>
#include <map>

namespace pathFinder {
class Timer {
private:
  double mergeTime{};
  double lookUpTime{};
  double searchTime{};
  double totalTime{};
  uint32_t numberOfLabelsToMerge{};
  uint32_t numberOfQueries = 1;

public:
  Timer() = default;
  explicit Timer(uint32_t numberOfQueries);
  void addMergeTime(double time);
  void addLookUpTime(double time);
  void addSearchTime(double time);
  void addTotalTime(double time);
  void addNumberOfLabelsToMerge(uint32_t labelsToMerge);
  double getAverageMergeTime();
  double getAverageLookUpTime();
  double getAverageSearchTime();
  double getAverageTotalTime();
  void resetAll();
};
} // namespace pathFinder