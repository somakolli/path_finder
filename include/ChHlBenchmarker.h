#ifndef CHHLBENCHMARKER_H
#define CHHLBENCHMARKER_H

#include "CHDijkstra.h"

namespace pathFinder{
class ChHlBenchmarker {
private:
	CHGraph& graph;
	size_t numberOfQueriesPerLevel = 1000;
public:
	explicit ChHlBenchmarker(CHGraph& graph);
    void compareSpeed(boost::filesystem::path benchFilePath);

};
}

#endif