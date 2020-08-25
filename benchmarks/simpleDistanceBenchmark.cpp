//
// Created by sokol on 11.08.20.
//
#include <fstream>
#include <iostream>
#include <path_finder/helper/Benchmarker.h>
int main(int argc, char* argv[]) {
  std::string dataPath = "/home/sokol/Uni/master-arbeit/build/data";
  int level = -1;
  int numberOfQueries = 100;
  for (int i = 1; i < argc; ++i) {
    std::string option = argv[i];
    if (option == "-l"){
      level = std::stoi(argv[++i]);
    }
    else if (option == "-n") {
      numberOfQueries = std::stoi(argv[++i]);
    }
    else if(option == "-f") {
      dataPath = argv[++i];
    }
    else {
      std::cerr << "Unknown option: " << option << '\n';
      return 1;
    }
  }
  pathFinder::Benchmarker benchmarker(dataPath, dataPath);
  if(level >= 0) {
    benchmarker.benchmarkLevel(level, numberOfQueries);
    benchmarker.benchmarkCHDijkstra(numberOfQueries);
  }
  else {
    std::vector<pathFinder::Benchmarker::BenchResult> hybResult = benchmarker.benchmarkAllLevel(numberOfQueries);
    pathFinder::RoutingResultTimingInfo chResult = benchmarker.benchmarkCHDijkstra(numberOfQueries * 10);
    std::ofstream plotOutputFile;
    plotOutputFile.open ("plot.txt");
    pathFinder::Benchmarker::printRoutingResultForOctave(plotOutputFile, hybResult, chResult.distanceTime);
    plotOutputFile.close();
  }
  return 0;
}