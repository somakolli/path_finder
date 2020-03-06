//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHGRAPH_H
#define ALG_ENG_PROJECT_CHGRAPH_H

#include "Graph.h"
#include <algorithm>

namespace pathFinder{
struct CHNode : Node {
    Level level;
};
template <class NodeVector , class EdgeVector, class OffsetVector>
class CHGraph {
private:
    NodeVector nodes;
    EdgeVector backEdges;
    OffsetVector backOffset;
public:
    CHGraph(NodeVector& nodes, EdgeVector& edges,EdgeVector& backEdges, OffsetVector& offset,OffsetVector& backOffset, size_t numberOfNodes);

    CHGraph();

    OffsetVector offset;
    EdgeVector edges;
    size_t numberOfNodes;
    [[nodiscard]] MyIterator<const Edge *> edgesFor(NodeId node, EdgeDirection direction) {
        switch (direction) {
            case FORWARD:
                return {&edges[offset[node]], &edges[offset[node + 1]]};
            case BACKWARD:
                return {&backEdges[backOffset[node]], &backEdges[backOffset[node + 1]]};
        }
        return {&edges[offset[node]], &edges[offset[node + 1]]};
    }
    Level getLevel(NodeId nodeId) {
        return nodes[nodeId].level;
    }
    void sortByLevel(std::vector<CHNode>& sortedNodes);
    NodeVector& getNodes();
    EdgeVector & getForwardEdges();
    OffsetVector& getForwardOffset();
    EdgeVector & getBackEdges();
    OffsetVector& getBackOffset();
};
template <class NodeVector , class EdgeVector, class OffsetVector>
void pathFinder::CHGraph<NodeVector, EdgeVector, OffsetVector>::sortByLevel(std::vector<CHNode> &sortedNodes) {
    sortedNodes.reserve(nodes.size());
    for(const auto& node : nodes)
        sortedNodes.emplace_back(node);
    std::sort(sortedNodes.begin(), sortedNodes.end(),  [](const auto& node1,const auto& node2) -> bool{
        return node1.level == node2.level? node1.id < node2.id : node1.level > node2.level;
    });
}
template <class NodeVector , class EdgeVector, class OffsetVector>
NodeVector &pathFinder::CHGraph<NodeVector, EdgeVector, OffsetVector>::getNodes() {
    return nodes;
}
template <class NodeVector , class EdgeVector, class OffsetVector>
EdgeVector &pathFinder::CHGraph<NodeVector, EdgeVector, OffsetVector>::getBackEdges() {
    return backEdges;
}
template <class NodeVector , class EdgeVector, class OffsetVector>
OffsetVector &pathFinder::CHGraph<NodeVector, EdgeVector, OffsetVector>::getBackOffset() {
    return backOffset;
}
template <class NodeVector , class EdgeVector, class OffsetVector>
EdgeVector &pathFinder::CHGraph<NodeVector, EdgeVector, OffsetVector>::getForwardEdges() {
    return edges;
}
template <class NodeVector , class EdgeVector, class OffsetVector>
OffsetVector &pathFinder::CHGraph<NodeVector, EdgeVector, OffsetVector>::getForwardOffset() {
    return offset;
}

template<class NodeVector, class EdgeVector, class OffsetVector>
CHGraph<NodeVector, EdgeVector, OffsetVector>::CHGraph(NodeVector& nodes, EdgeVector& edges,EdgeVector& backEdges,
                                                       OffsetVector& backOffset, OffsetVector& offset, size_t numberOfNodes
                                                       ):nodes(nodes), backEdges(backEdges),
                                                                         backOffset(backOffset), offset(offset),
                                                                         edges(edges), numberOfNodes(numberOfNodes) {}

template<class NodeVector, class EdgeVector, class OffsetVector>
CHGraph<NodeVector, EdgeVector, OffsetVector>::CHGraph() {}
}
#endif //ALG_ENG_PROJECT_CHGRAPH_H
