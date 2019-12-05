//
// Created by sokol on 24.11.19.
//
#include <sstream>
#include "../include/GraphStats.h"
#include "../include/CHDijkstra.h"

pathFinder::GraphStats::GraphStats(pathFinder::CHGraph &graph) : graph(graph) {}

std::uint32_t pathFinder::GraphStats::getDegree(pathFinder::NodeId id) {
    return graph.edgesFor(id, EdgeDirection::BACKWARD).size()
                + graph.edgesFor(id, EdgeDirection::FORWARD).size();
}

float pathFinder::GraphStats::getAverageDegree() {
    size_t accumulator = 0;
    for(auto node: graph.getNodes()) {
        accumulator += getDegree(node.id);
    }
    return (float)accumulator / graph.numberOfNodes;
}

std::uint32_t pathFinder::GraphStats::getNodeCountWithLevelSmallerThan(pathFinder::Level level) {
    size_t count = 0;
    for(auto node : graph.getNodes()) {
        if(node.level < level)
            ++count;
    }
    return count;
}

float pathFinder::GraphStats::getAverageHopCountUntilLevel(pathFinder::Level level, EdgeDirection direction) {
    CHDijkstra dijkstra = CHDijkstra(graph);
    size_t totalHopCount = 0;
    for(const auto& node : graph.getNodes()) {
        totalHopCount += dijkstra.hopCountUntilLevel(node.id, level, direction);
    }
    return (float)totalHopCount / graph.numberOfNodes;
}

pathFinder::Level pathFinder::GraphStats::getHighestLevel() {
    Level highestLevel = 0;
    for(const auto& node : graph.getNodes())
        if(node.level > highestLevel)
            highestLevel = node.level;
    return highestLevel;
}

void pathFinder::GraphStats::printStats(std::ostream& ss) {
    ss << "total node size: " << graph.numberOfNodes << "\n";
    ss << "total edge size: " << graph.edges.size() << "\n";
    ss << "average degree: " << getAverageDegree() << "\n";
    Level highestLevel = getHighestLevel();
    ss << "highest level: " << highestLevel << "\n";
    std::string seperator = "------------------------------------------------------";
    ss << seperator << "\n";
    ss << "number of nodes < level\n";
    for(Level level = 1; level < 50; ++level) {
        size_t nodeCount = getNodeCountWithLevelSmallerThan(level);
        ss << nodeCount << ',' << level << ',' << (float)nodeCount / graph.numberOfNodes <<  '\n';
    }
    ss << seperator << '\n';
    ss << "average hopcount\n";
    for(Level level = 1; level < 50; ++level) {
        ss <<level << "," << getAverageHopCountUntilLevel(level, EdgeDirection::FORWARD) << "\n";
    }
}


