#include "path_finder/graphs/Graph.h"
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
  double averageLabelSize{};
  uint32_t numberOfQueries = 1;

public:
  Timer() = default;
  explicit Timer(uint32_t numberOfQueries);
  void addMergeTime(double time);
  void addLookUpTime(double time);
  void addSearchTime(double time);
  void addTotalTime(double time);
  void addNumberOfLabelsToMerge(uint32_t labelsToMerge);
  void addAverageLabelSize(double averageLabelSize);
  double getAverageMergeTime() const;
  double getAverageLookUpTime() const;
  double getAverageSearchTime() const;
  double getAverageTotalTime() const;
  double getAverageNumbersOfLabels() const;
  double getAverageItemNumber() const;
  void resetAll();
};
} // namespace pathFinder