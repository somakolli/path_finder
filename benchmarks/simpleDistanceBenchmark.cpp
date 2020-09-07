//
// Created by sokol on 11.08.20.
//
#include <fstream>
#include <iostream>
#include <path_finder/helper/Benchmarker.h>
#include "CLI11.hpp"
int main(int argc, char* argv[]) {
  CLI::App app("routing-file-creator");

  std::string routingDataPath;
  app.add_option("-f, --routingData", routingDataPath,
                 "Path to routing data!")->required();
  int numberOfQueries;
  app.add_option("-n, --numberOfQueries", numberOfQueries,
                 "number of queries query query run!")->required();
  std::string outputPath;
  app.add_option("-o, --outPutPathForOcatve", outputPath,
                 "Into this file the results will be printed in octave syntax")->required();
  bool hybrid = false;
  app.add_flag("-a, --hybrid", hybrid);
  bool chDijkstra = false;
  app.add_flag("-d, --chDijkstra", chDijkstra);
  CLI11_PARSE(app, argc, argv);


  if(chDijkstra) {
    std::cout << routingDataPath << '\n';
    auto chDijkstraData = pathFinder::FileLoader::loadCHDijkstraShared(routingDataPath);
    pathFinder::Benchmarker::benchmarkCHDijkstra(*chDijkstraData, numberOfQueries);
  }
  if(hybrid) {
    auto hubLabels = pathFinder::FileLoader::loadHubLabelsShared(routingDataPath);
    auto result = pathFinder::Benchmarker::benchmarkAllLevel(*hubLabels, numberOfQueries);
    std::ofstream plotOutputFile;
    plotOutputFile.open (outputPath);
    pathFinder::Benchmarker::printRoutingResultForOctave(plotOutputFile, result);
    plotOutputFile.close();
  }
  return 0;
}