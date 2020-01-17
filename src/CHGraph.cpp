//
// Created by sokol on 03.10.19.
//

#include <algorithm>
#include "../include/CHGraph.h"
#include "../include/MmapVector.h"
#include <sys/types.h>
#include <sys/mman.h>


template<typename edgeVectorT, typename nodeVectorT, typename offsetVectorT>
nodeVectorT& pathFinder::CHGraph<edgeVectorT, nodeVectorT, offsetVectorT>::nodes() {
    return _nodes;
}

template<typename edgeVectorT, typename nodeVectorT, typename offsetVectorT>
edgeVectorT& pathFinder::CHGraph<edgeVectorT, nodeVectorT, offsetVectorT>::backEdges() {
    return _backEdges;
}

template<typename edgeVectorT, typename nodeVectorT, typename offsetVectorT>
offsetVectorT& pathFinder::CHGraph<edgeVectorT, nodeVectorT, offsetVectorT>::backOffset() {
    return _backOffset;
}

template<typename edgeVectorT, typename nodeVectorT, typename offsetVectorT>
edgeVectorT& pathFinder::CHGraph<edgeVectorT, nodeVectorT, offsetVectorT>::edges() {
    return _edges;
}

template<typename edgeVectorT, typename nodeVectorT, typename offsetVectorT>
offsetVectorT& pathFinder::CHGraph<edgeVectorT, nodeVectorT, offsetVectorT>::offset() {
    return _offset;
}

template<typename edgeVectorT, typename nodeVectorT, typename offsetVectorT>
void pathFinder::CHGraph<edgeVectorT, nodeVectorT, offsetVectorT>::sortByLevel(std::vector<CHNode> &sortedNodes) {
    sortedNodes.reserve(nodes().size());
    for(const auto& node : nodes())
        sortedNodes.emplace_back(node);
    std::sort(sortedNodes.begin(), sortedNodes.end(),  [](const auto& node1,const auto& node2) -> bool{
        return node1.level == node2.level? node1.id < node2.id : node1.level > node2.level;
    });
}
template class pathFinder::CHGraph<std::vector<pathFinder::Edge>, std::vector<pathFinder::CHNode>, std::vector<uint32_t>>;
template class pathFinder::CHGraph<pathFinder::MmapVector<pathFinder::Edge>, pathFinder::MmapVector<pathFinder::CHNode>, pathFinder::MmapVector<uint32_t>>;