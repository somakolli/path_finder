//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHGRAPH_H
#define ALG_ENG_PROJECT_CHGRAPH_H

#include "Graph.h"

namespace algEng{
struct CHNode : Node {
    Level level;
};
class CHGraph : public Graph {
private:
    std::vector<CHNode> nodes;
    std::vector<Edge> backEdges;
    std::vector<NodeId> backOffset;
public:


    [[nodiscard]] MyIterator<const Edge *> edgesFor(NodeId node, EdgeDirection direction) const {
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
    std::vector<CHNode>& getNodes();
    std::vector<Edge>& getBackEdges();
    std::vector<NodeId>& getBackOffset();
};

}
#endif //ALG_ENG_PROJECT_CHGRAPH_H
