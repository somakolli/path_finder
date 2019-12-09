//
// Created by sokol on 03.10.19.
//

#include <algorithm>
#include "../include/CHGraph.h"


void pathFinder::CHGraph::sortByLevel(std::vector<CHNode> &sortedNodes) {
    sortedNodes.reserve(nodes.size());
    for(const auto& node : nodes)
        sortedNodes.emplace_back(node);
    std::sort(sortedNodes.begin(), sortedNodes.end(),  [](const auto& node1,const auto& node2) -> bool{
        return node1.level == node2.level? node1.id < node2.id : node1.level > node2.level;
    });
}

std::vector<pathFinder::CHNode>& pathFinder::CHGraph::getNodes() {
    return nodes;
}

pathFinder::Graph::edgeVector &pathFinder::CHGraph::getBackEdges() {
    return backEdges;
}

std::vector<pathFinder::NodeId> &pathFinder::CHGraph::getBackOffset() {
    return backOffset;
}
