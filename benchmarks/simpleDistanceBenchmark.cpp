//
// Created by sokol on 11.08.20.
//
#include <path_finder/helper/Benchmarker.h>
#include <iostream>
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
    else {
      std::cerr << "Unknown option: " << option << '\n';
      return 1;
    }
  }
  pathFinder::Benchmarker benchmarker(dataPath, dataPath);
  if(level >= 0)
    benchmarker.benchmarkLevel(level, numberOfQueries);
  else
    benchmarker.benchmarkAllLevel(numberOfQueries);
  return 0;
}