#ifndef CHHLBENCHMARKER_H
#define CHHLBENCHMARKER_H

#include "CHDijkstra.h"
#include "GraphStats.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace pathFinder{
template <class Graph>
class ChHlBenchmarker {
private:
	Graph& graph;
	size_t numberOfQueriesPerLevel = 100;
public:
	explicit ChHlBenchmarker(Graph& graph);
    void compareSpeed(boost::filesystem::path benchFilePath, Level untilLevel);

};
template <class Graph>
pathFinder::ChHlBenchmarker<Graph>::ChHlBenchmarker(Graph &graph) : graph(graph) {
}

template <class Graph>
void
pathFinder::ChHlBenchmarker<Graph>::compareSpeed(boost::filesystem::path benchFilePath, Level untilLevel) {
    //CHDijkstra chDijkstra(graph);
    boost::filesystem::ofstream ofs{benchFilePath};
    GraphStats<Graph> gs(graph);
    boost::random::mt19937 rng;
    boost::random::uniform_int_distribution<> node(0, graph.numberOfNodes-1);
    Level maxLevel = gs.getHighestLevel();
    ofs << "level,searchTime,mergeTime,loopUpTime,totalTime(µs)\n";
    std::vector<CHNode> sortedNodes;
    graph.sortByLevel(sortedNodes);
    std::vector<Distance > cost;
    cost.reserve(graph.getNodes().size());
    while(cost.size() < graph.getNodes().size())
        cost.push_back(MAX_DISTANCE);
    for(int i = maxLevel + 1; i >= untilLevel; --i) {
        HubLabels<HubLabelStore<std::vector>, Graph> hubLabels(graph, i, sortedNodes, cost);
        double totalTime = 0;
        double searchTime = 0;
        double mergeTime = 0;
        double lookUpTime = 0;
        for(int j = 0; j < numberOfQueriesPerLevel; ++j) {
            auto start = std::chrono::high_resolution_clock::now();
            hubLabels.getShortestDistance(node(rng), node(rng), searchTime, mergeTime, lookUpTime).value();
            auto finish = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
            totalTime += elapsed.count();
        }
        ofs << i << "," << searchTime/numberOfQueriesPerLevel << "," << mergeTime/numberOfQueriesPerLevel << "," << lookUpTime/numberOfQueriesPerLevel << "," << totalTime/numberOfQueriesPerLevel << ","  << hubLabels.getSpaceConsumption() << std::endl;
        std::cout << "finished level: " << i << "with average time: " << totalTime/numberOfQueriesPerLevel << "," << searchTime/numberOfQueriesPerLevel << "," << mergeTime/numberOfQueriesPerLevel << "," << lookUpTime/numberOfQueriesPerLevel << std::endl;
    }
}
}

#endif