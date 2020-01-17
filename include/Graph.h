//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_GRAPH_H
#define ALG_ENG_PROJECT_GRAPH_H

#include <cstdint>
#include <limits>
#include <iostream>
#include "vector"
#include "ostream"
#include "MmapVector.h"

#define Print(x) std::cout << x << '\n';
#ifndef NDEBUG
    #define Debug(x);
#else
    #define Debug(x) std::cout << x << '\n';
#endif

namespace pathFinder{
using NodeId = uint32_t ;
using Lat = float;
using Lng = float;
using Distance = uint32_t ;
using Level = uint16_t ;

struct LatLng {
    Lat lat;
    Lng lng;
};
constexpr Distance MAX_DISTANCE = std::numeric_limits<Distance>::max();
enum EdgeDirection {
        FORWARD = true,
        BACKWARD = false
};
class Edge {
public:
	NodeId source;
	NodeId target;
	NodeId distance;
	Edge(NodeId source, NodeId target, Distance distance) : source(source), target(target),
															distance(distance){}
	Edge() = default;
	friend std::ostream &operator<<(std::ostream & Str, const Edge & edge) {
		Str << "source: " << edge.source << ' ' << "target: " << edge.target << " distance: " << edge.distance;
		return Str;
	}
}__attribute__((packed));
class Node {
public:
    NodeId id;
    Lat lat;
    Lng lng;
    double quickBeeLine(const LatLng& other) const;
}__attribute__((packed));
template <typename MyPointerType>
class MyIterator{
private:
    MyPointerType _begin;
    MyPointerType _end;
public:
    MyIterator(MyPointerType begin, MyPointerType end) : _begin(begin), _end(end) {}

    MyPointerType begin() {return _begin;};
    MyPointerType end() {return _end;};
    size_t size() {return _end - _begin;}
};
template <typename edgeVector, typename nodeVector, typename offsetVector>
class Graph {
private:
    edgeVector _edges;
    nodeVector _nodes;
    offsetVector _offset;
public:
    uint32_t numberOfNodes{};
    edgeVector& edges();
    nodeVector& nodes();
    offsetVector& offset();
	Graph() = default;
	~Graph() = default;
	const friend std::ostream &operator<<(std::ostream & Str, Graph graph) {
		auto i = 0;

		for(auto edge: graph.edges()) {
			Str << edge << '\n';
		}
		for(i = 0; i < graph.offset().size(); ++i) {
			Str << i << ":" << graph.offset()[i] << std::endl;
		}
		Str << "nodes: " << graph.numberOfNodes << std:: endl;
		Str << "edges: " << graph.edges().size() << std::endl;
		Str << "offset: " << graph.offset().size() << std::endl;
		return Str;
	}
	MyIterator<Edge*> edgesFor(NodeId node) {
	    return {&_edges[_offset[node]], &_edges[_offset[node+1]]};
	}
	NodeId getNodeId(LatLng latLng) const;
	LatLng getLatLng(NodeId nodeId) const;
};
// used for dijkstra PQ
class CostNode {
public:
	NodeId id;
	Distance cost;
    CostNode() = default;

    bool operator<(const CostNode& rhs) const {
		return cost > rhs.cost;
	}
	CostNode(size_t id, size_t cost): id(id), cost(cost) {}
};
}
#endif //ALG_END_PROJECT_GRAPH_H
