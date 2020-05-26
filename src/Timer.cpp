#include "path_finder/Timer.h"

void pathFinder::Timer::addMergeTime(double time) { mergeTime += time; }

void pathFinder::Timer::addLookUpTime(double time) { lookUpTime += time; }

void pathFinder::Timer::addSearchTime(double time) { searchTime += time; }

void pathFinder::Timer::addTotalTime(double time) { totalTime += time; }

double pathFinder::Timer::getAverageMergeTime() const {
  return mergeTime / numberOfQueries;
}

double pathFinder::Timer::getAverageLookUpTime() const {
  return lookUpTime / numberOfQueries;
}

double pathFinder::Timer::getAverageSearchTime() const {
  return searchTime / numberOfQueries;
}

pathFinder::Timer::Timer(uint32_t numberOfQueries)
    : numberOfQueries(numberOfQueries) {}

void pathFinder::Timer::addNumberOfLabelsToMerge(uint32_t labelsToMerge) {
  numberOfLabelsToMerge += labelsToMerge;
}

double pathFinder::Timer::getAverageTotalTime() const {
  return totalTime / numberOfQueries;
}

void pathFinder::Timer::resetAll() { *this = Timer(numberOfQueries); }
void pathFinder::Timer::addAverageLabelSize(double labelSize) {
  averageLabelSize += labelSize;
}
double pathFinder::Timer::getAverageNumbersOfLabels() const {
  return  float(numberOfLabelsToMerge) / numberOfQueries;
}
double pathFinder::Timer::getAverageItemNumber() const {
  return float(averageLabelSize) / numberOfQueries;
}
