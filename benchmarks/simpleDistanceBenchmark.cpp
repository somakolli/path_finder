//
// Created by sokol on 11.08.20.
//
#include <path_finder/helper/Benchmarker.h>
#include <iostream>
int main() {
  std::string dataPath = "/home/sokol/Uni/master-arbeit/build/data";
  pathFinder::Benchmarker benchmarker(dataPath, dataPath);
  benchmarker.runPathBenchmark(1);
}