//
// Created by sokol on 11.08.20.
//
#include <fstream>
#include <iostream>
#include <path_finder/helper/Benchmarker.h>
#include "CLI11.hpp"
auto main(int argc, char* argv[]) -> int {
  CLI::App app("routing-benchmarker");

  std::string routingDataPath;
  app.add_option("-f, --routingData", routingDataPath,
                 "Path to routing data!")->required();
  int numberOfQueries;
  app.add_option("-n, --numberOfQueries", numberOfQueries,
                 "number of queries query query run!")->required();
  std::string outputPath;
  app.add_option("-o, --outPutPathForOctave", outputPath,
                 "Into this file the results will be printed in octave syntax")->required();
  bool hybrid = false;
  app.add_flag("-a, --hybrid", hybrid);
  bool chDijkstra = false;
  app.add_flag("-d, --chDijkstra", chDijkstra);
  bool interactive = false;
  app.add_flag("-i, --interactive", interactive);
  CLI11_PARSE(app, argc, argv);

  if(chDijkstra) {
    std::cout << "loading files: " << routingDataPath << '\n';
    auto chDijkstraData = pathFinder::FileLoader::loadCHDijkstraShared(routingDataPath);
    if(!interactive) {
      pathFinder::Benchmarker::benchmarkCHDijkstra(*chDijkstraData, numberOfQueries);
    } else {
      while(true) {
        std::cout << "press any key to continue... " << '\n';
        getchar();
        pathFinder::Benchmarker::benchmarkCHDijkstra(*chDijkstraData, numberOfQueries);
      }
    }
  }
  if(hybrid) {
    std::cout << "loading files: " << routingDataPath << '\n';
    auto hubLabels = pathFinder::FileLoader::loadHubLabelsShared(routingDataPath);
    if(!interactive) {
      auto result = pathFinder::Benchmarker::benchmarkAllLevel(*hubLabels, numberOfQueries);
      std::ofstream plotOutputFile;
      plotOutputFile.open (outputPath);
      pathFinder::Benchmarker::printRoutingResultForOctave(plotOutputFile, result);
      plotOutputFile.close();
    } else {
      while(true) {
        auto dijkstra = pathFinder::FileLoader::loadCHDijkstraShared(routingDataPath);
        std::cout << "enter level: " << '\n';
        std::string in;
        std::cin >> in;
        pathFinder::Level level = std::stoi(in);
        pathFinder::Benchmarker::benchmarkLevel(*hubLabels, level, numberOfQueries);
        auto additionalSpace = hubLabels->m_spaceMeasurer.getSpaceConsumption(level);
        std::cout << "additional Space Required: " << pathFinder::Static::humanReadable(additionalSpace) << '\n';
        pathFinder::Benchmarker::benchmarkCHDijkstra(*dijkstra, numberOfQueries);
      }

    }
  }

  return 0;
}