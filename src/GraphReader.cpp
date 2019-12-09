//
// Created by sokol on 02.10.19.
//
#include "../include/GraphReader.h"
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include <fcntl.h>
#include <iostream>
#include <boost/algorithm/string.hpp>

namespace io = boost::iostreams;
void pathFinder::GraphReader::readFmiFile(pathFinder::Graph &graph, const std::string& filepath) {
	uint32_t numberOfEdges{};
	int fdr = open(filepath.data(), O_RDONLY);
	if (fdr >= 0) {
		io::file_descriptor_source fdDevice(fdr, io::file_descriptor_flags::close_handle);
		io::stream <io::file_descriptor_source> in(fdDevice);
		if (fdDevice.is_open()) {
			std::string line;
			// ignore first 5 lines
			for(auto i = 0; i < 5; ++i) {
				std::getline(in, line);
			}
			in >> graph.numberOfNodes;
			in >> numberOfEdges;
			uint32_t type;
			uint32_t maxSpeed;
            int i = graph.numberOfNodes + 1;
            Node node{};
            long id2;
            double elevation;
			while (--i > 0 && in >> node.id >> id2 >> node.latLng.lat >> node.latLng.lng >> elevation){
				graph.nodes.push_back(node);
			}
			i = numberOfEdges + 1;
			Edge edge{};
			while (--i > 0 && in >> edge.source >> edge.target >> edge.distance >> type >> maxSpeed) {
				graph.edges.push_back(edge);
			}
			fdDevice.close();
		}
	}
	buildOffset(graph.edges, graph.offset);
}

void pathFinder::GraphReader::buildOffset(const Graph::edgeVector & edges, std::vector<NodeId>& offset) {
	offset.clear();
	if(edges.empty() )
		return;
	NodeId numberOfNodes = edges[edges.size()-1].source + 1;
	offset.reserve(numberOfNodes+1);
	while(offset.size() <= numberOfNodes) {
		offset.emplace_back(0);
	}
	offset[numberOfNodes] = edges.size();
	for(int i = edges.size()-1; i >= 0; --i ){
		offset[edges[i].source] = i;
	}
	for(int i = edges[0].source + 1; i < offset.size()-2; ++i) {

		if(offset[i]==0) {
			size_t j = i+1;
			while(offset[j]==0){
				++j;
			}
			size_t offsetToSet = offset[j];
			--j;
			size_t firstNullPosition = i;
			while(j >= firstNullPosition) {
				offset[j] = offsetToSet;
				--j;
				++i;
			}
		}
	}
	offset[0] = 0;
	offset[offset.size()-1] = edges.size();
}

void pathFinder::GraphReader::readCHFmiFile(pathFinder::CHGraph &graph, const std::string &filepath) {
    uint32_t numberOfEdges{};
    int fdr = open(filepath.data(), O_RDONLY);
    if (fdr >= 0) {
        io::file_descriptor_source fdDevice(fdr, io::file_descriptor_flags::close_handle);
        io::stream <io::file_descriptor_source> in(fdDevice);
        if (fdDevice.is_open()) {
            std::string line;
            // ignore first 5 lines
            for(auto i = 0; i < 10; ++i) {
                std::getline(in, line);
            }
            in >> graph.numberOfNodes;
            in >> numberOfEdges;
            uint32_t type;
            uint32_t maxSpeed;
            uint32_t child1;
            uint32_t child2;
            uint64_t osmId;
            Lat lat;
            Lng lng;
            uint32_t el;
            int i = graph.numberOfNodes + 1;
            CHNode node{};
            while(--i > 0 && in >> node.id >> osmId >> lat >> lng >> el >> node.level) {
                graph.getNodes().emplace_back(node);
            }
            i = numberOfEdges + 1;
            Edge edge{};
            while (--i > 0 && in >> edge.source >> edge.target >> edge.distance >> type >> maxSpeed >> child1 >> child2) {
                graph.edges.push_back(edge);
            }
            fdDevice.close();
        }
    }
    buildOffset(graph.edges, graph.offset);
    buildBackEdges(graph.edges, graph.getBackEdges());
    buildOffset(graph.getBackEdges(), graph.getBackOffset());
}

void pathFinder::GraphReader::buildBackEdges(const Graph::edgeVector &forwardEdges, Graph::edgeVector &backEdges) {
    for(const auto& edge: forwardEdges) {
        Edge backWardEdge = edge;
        backWardEdge.source = edge.target;
        backWardEdge.target = edge.source;
        backEdges.push_back(backWardEdge);
    }
    sortEdges(backEdges);
}

void pathFinder::GraphReader::sortEdges(Graph::edgeVector &edges) {
    std::sort(edges.begin(), edges.end(), [](const auto& edge1,const auto& edge2) -> bool{
        return (edge1.source == edge2.source) ? edge1.target <= edge2.target : edge1.source < edge2.source;
    });
}
