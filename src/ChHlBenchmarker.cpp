#include "../include/ChHlBenchmarker.h"
#include "../include/GraphStats.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

pathFinder::ChHlBenchmarker::ChHlBenchmarker(pathFinder::CHGraph &graph) : graph(graph) {
}

void
pathFinder::ChHlBenchmarker::compareSpeed(boost::filesystem::path benchFilePath) {
    //CHDijkstra chDijkstra(graph);
    boost::filesystem::ofstream ofs{benchFilePath};
    GraphStats gs(graph);
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> node(0, graph.numberOfNodes-1);
    Level maxLevel = gs.getHighestLevel();
    ofs << "level,time(µs)\n";
    for(int i = maxLevel + 1; i >= 0; --i) {
        HubLabels hubLabels(graph, i);
        double totalTime = 0;
        for(int j = 0; j < numberOfQueriesPerLevel; ++j) {
            auto start = std::chrono::high_resolution_clock::now();
            hubLabels.getShortestDistance(node(rng), node(rng)).value();
            auto finish = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
            totalTime += elapsed.count();
        }
        ofs << i << "," << totalTime/numberOfQueriesPerLevel << "\n";
        std::cout << "finished level: " << i << "with average time: " << totalTime/numberOfQueriesPerLevel << std::endl;
    }
}
