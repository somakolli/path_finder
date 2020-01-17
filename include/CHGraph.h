//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHGRAPH_H
#define ALG_ENG_PROJECT_CHGRAPH_H

#include "Graph.h"

namespace pathFinder{
struct CHNode : Node {
    Level level;
}__attribute__((packed));
template<typename edgeVectorT, typename nodeVectorT, typename offsetVectorT>
class CHGraph {
private:
    edgeVectorT _edges;
    offsetVectorT _offset;
    nodeVectorT _nodes;
    edgeVectorT _backEdges;
    offsetVectorT _backOffset;
public:
    edgeVectorT& edges();
    offsetVectorT& offset();
    nodeVectorT& nodes();
    edgeVectorT& backEdges();
    offsetVectorT& backOffset();
    uint64_t numberOfNodes{};
    [[nodiscard]] MyIterator<Edge *> edgesFor(NodeId node, EdgeDirection direction)  {
        switch (direction) {
            case FORWARD:
                return {&(_edges[_offset[node]]), &_edges[_offset[node + 1]]};
            case BACKWARD:
                return {&_backEdges[_backOffset[node]], &_backEdges[_backOffset[node + 1]]};
        }
        return {&_edges[_offset[node]], &_edges[_offset[node + 1]]};
    }
    Level getLevel(NodeId nodeId) {
        return nodes()[nodeId].level;
    }
    void sortByLevel(std::vector<CHNode>& sortedNodes);
};
using DiskGraph = CHGraph<MmapVector<Edge>, MmapVector<CHNode>, MmapVector<uint32_t>>;
using RamGraph = CHGraph<std::vector<Edge>, std::vector<CHNode>, std::vector<uint32_t>>;
}
#endif //ALG_ENG_PROJECT_CHGRAPH_H
