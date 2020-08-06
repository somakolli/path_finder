//
// Created by sokol on 02.10.19.
//

#ifndef ALG_ENG_PROJECT_GRAPH_H
#define ALG_ENG_PROJECT_GRAPH_H

#include "ostream"
#include "vector"
#include <cstdint>
#include <iostream>
#include <limits>

#ifndef NDEBUG
#define Debug(x) ;
#else
#define Debug(x) std::cout << x << '\n';
#endif

namespace pathFinder {
using NodeId = uint32_t;
using Lat = float;
using Lng = float;
using Distance = uint32_t;
using Level = uint16_t;

struct LatLng {
  Lat lat;
  Lng lng;

  bool operator==(LatLng other) const {
    return other.lat == lat && other.lng == lng;
  }
};

constexpr Distance MAX_DISTANCE = std::numeric_limits<Distance>::max();
enum EdgeDirection { FORWARD = true, BACKWARD = false };
class Edge {
public:
  NodeId source;
  NodeId target;
  NodeId distance;
  Edge(NodeId source, NodeId target, Distance distance)
      : source(source), target(target), distance(distance) {}
  Edge() = default;
  friend std::ostream &operator<<(std::ostream &Str, const Edge &edge) {
    Str << "source: " << edge.source << ' ' << "target: " << edge.target
        << " distance: " << edge.distance;
    return Str;
  }
};
class Node {
public:
  NodeId id;
  LatLng latLng;
  double quickBeeLine(const LatLng &other) const;
  double quickBeeLine(const Node &other) const;
  double euclid(const Node &other) const;

};
template <typename MyPointerType> class MyIterator {
private:
  MyPointerType _begin;
  MyPointerType _end;

public:
  MyIterator(MyPointerType begin, MyPointerType end)
      : _begin(begin), _end(end) {}
  bool empty() const { return _begin == _end; };
  MyPointerType begin() { return _begin; };
  MyPointerType begin() const { return _begin; };
  MyPointerType end() { return _end; };
  MyPointerType end() const { return _end; };
  size_t size() { return _end - _begin; }

};
class Graph {
public:
  typedef std::vector<Node> nodeVector;
  typedef std::vector<Edge> edgeVector;

private:
public:
  // typedef stxxl::VECTOR_GENERATOR<Node>::result nodeVector;
  // typedef stxxl::VECTOR_GENERATOR<Edge>::result edgeVector;
  uint32_t numberOfNodes{};
  std::vector<uint32_t> offset;
  edgeVector edges;
  nodeVector nodes;
  Graph() = default;
  ~Graph() = default;
  const friend std::ostream &operator<<(std::ostream &Str, Graph graph) {
    auto i = 0;

    for (auto edge : graph.edges) {
      Str << edge << '\n';
    }
    for (i = 0; i < graph.offset.size(); ++i) {
      Str << i << ":" << graph.offset[i] << std::endl;
    }
    Str << "nodes: " << graph.numberOfNodes << std::endl;
    Str << "edges: " << graph.edges.size() << std::endl;
    Str << "offset: " << graph.offset.size() << std::endl;
    return Str;
  }
  MyIterator<const Edge *> edgesFor(NodeId node) const {
    return {&edges[offset[node]], &edges[offset[node + 1]]};
  }
  NodeId getNodeId(LatLng latLng) const;
  LatLng getLatLng(NodeId nodeId) const;
};
// used for dijkstra PQ
class CostNode {
public:
  NodeId id;
  Distance cost;
  NodeId previousNode;
  CostNode() = default;

  bool operator<(const CostNode &rhs) const { return cost > rhs.cost; }
  CostNode(NodeId id, size_t cost, NodeId previousNode)
      : id(id), cost(cost), previousNode(previousNode) {}

  bool operator==(const CostNode &rhs) const {
    return id == rhs.id && cost == rhs.cost && previousNode == rhs.previousNode;
  }
};
class PreviousReplacer {
private:
  NodeId currentNode;

public:
  explicit PreviousReplacer(NodeId id) {
    currentNode = id;
  }
  CostNode operator()  (CostNode costNode) const {
    if(costNode.previousNode == costNode.id){
      // the previous node of the label has not been set so replace it
      // with the current node
      return CostNode(costNode.id, costNode.cost, currentNode);
    }
    return costNode;
  }
  CostNode operator() (NodeId id, Distance cost, NodeId previousNode) {
    return (*this)(CostNode(id, cost, previousNode));
  }

};
} // namespace pathFinder
#endif // ALG_END_PROJECT_GRAPH_H
