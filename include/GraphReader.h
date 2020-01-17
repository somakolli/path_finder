//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_GRAPHREADER_H
#define ALG_ENG_PROJECT_GRAPHREADER_H


#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <fcntl.h>
#include <iostream>
#include "Graph.h"
#include "string"
#include "CHGraph.h"

namespace pathFinder{
class GraphReader{
public:

	static void readFmiFile(Graph<std::vector<Edge>, std::vector<Node>, std::vector<uint32_t>>& graph, const std::string& filepath){
        uint32_t numberOfEdges{};
        int fdr = open(filepath.data(), O_RDONLY);
        if (fdr >= 0) {
            boost::iostreams::file_descriptor_source fdDevice(fdr, boost::iostreams::file_descriptor_flags::close_handle);
            boost::iostreams::stream <boost::iostreams::file_descriptor_source> in(fdDevice);
            if (fdDevice.is_open()) {
                std::string line;
                // ignore first 5 lines
                for(auto i = 0; i < 5; ++i) {
                    std::getline(in, line);
                }
                in >> graph.numberOfNodes;
                in >> numberOfEdges;
                NodeId id;
                Lat lat;
                Lng lng;
                uint32_t type;
                uint32_t maxSpeed;
                int i = graph.numberOfNodes + 1;
                Node node{};
                long id2;
                double elevation;
                while (--i > 0 && in >> id >> id2 >> lat >> lng >> elevation){
                    graph.nodes().push_back(Node{id, lat, lng});
                }
                i = numberOfEdges + 1;
                NodeId source;
                NodeId target;
                Distance distance;
                while (--i > 0 && in >> source >> target >> distance >> type >> maxSpeed) {
                    graph.edges().push_back(Edge{source, target, distance});
                }
                fdDevice.close();
            }
        }
        buildOffset(graph.edges(), graph.offset());
	};
    static void readCHFmiFile(RamGraph & chGraph, const std::string& filepath, DiskGraph& diskGraph);
private:
	static void buildOffset(const std::vector<Edge> & edges, std::vector<NodeId>& offset);
	static void buildBackEdges(const std::vector<Edge> & forwardEdges, std::vector<Edge> & backEdges);
	static void sortEdges(std::vector<Edge> & edges);
};
}

#endif //ALG_ENG_PROJECT_GRAPHREADER_H
