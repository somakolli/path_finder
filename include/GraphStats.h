//
// Created by sokol on 24.11.19.
//

#ifndef ALG_ENG_PROJECT_GRAPHSTATS_H
#define ALG_ENG_PROJECT_GRAPHSTATS_H

#include "CHGraph.h"

namespace pathFinder {
    class GraphStats {
        CHGraph& graph;
    public:
        explicit GraphStats(CHGraph &graph);
        std::uint32_t getDegree(NodeId id);
        float getAverageDegree();
        std::uint32_t getNodeCountWithLevelSmallerThan(Level level);
        float getAverageHopCountUntilLevel(Level level, EdgeDirection direction);
        Level getHighestLevel();
        void printStats(std::ostream& ss);
    };
}
#endif //ALG_ENG_PROJECT_GRAPHSTATS_H
