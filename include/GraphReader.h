//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_GRAPHREADER_H
#define ALG_ENG_PROJECT_GRAPHREADER_H

#include "Graph.h"
#include "string"
#include "CHGraph.h"

namespace pathFinder{
class GraphReader{
public:
	static void readFmiFile(Graph& graph, const std::string& filepath);
    static void readCHFmiFile(CHGraph& chGraph, const std::string& filepath);
private:
	static void buildOffset(const std::vector<Edge>& edges, std::vector<NodeId>& offset);
	static void buildBackEdges(const std::vector<Edge>& forwardEdges, std::vector<Edge>& backEdges);
	static void sortEdges(std::vector<Edge>& edges);
};
}

#endif //ALG_ENG_PROJECT_GRAPHREADER_H
