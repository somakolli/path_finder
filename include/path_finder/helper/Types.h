//
// Created by sokol on 07.07.20.
//

#pragma once

#include <boost/optional.hpp>
#include <fstream>
#include <vector>

namespace pathFinder {
using NodeId = uint32_t ;
using Lat = float;
using Lng = float;
using Distance = uint32_t ;
using Level = uint16_t;
using CellId_t = uint32_t;
constexpr Distance MAX_DISTANCE = std::numeric_limits<Distance>::max();
enum EdgeDirection { FORWARD = true, BACKWARD = false };
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
  auto operator[](size_t position) const {
    return *(begin() + position);
  }
};

struct OffsetElement {
  size_t position = 0;
  size_t size = 0;
};
struct LatLng {
  Lat lat;
  Lng lng;

  LatLng() = default;
  LatLng(const double lat, const double lng): lat(lat), lng(lng) {}
  bool operator==(LatLng other) const {
    return other.lat == lat && other.lng == lng;
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
struct CHNode : Node {
  Level level;
};
struct CHEdge : Edge {
  std::optional<NodeId> child1 = std::nullopt;
  std::optional<NodeId> child2 = std::nullopt;
};

struct CalcLabelTimingInfo{
  double mergeTime = 0;
  double graphSearchTime = 0;
  double lookUpTime = 0;

  void operator+=(const CalcLabelTimingInfo& other) {
    mergeTime += other.mergeTime;
    graphSearchTime += other.graphSearchTime;
    lookUpTime += other.lookUpTime;
  }
  void operator/=(double other) {
    mergeTime /= other;
    lookUpTime /= other;
    graphSearchTime /= other;
  }
  friend std::ostream& operator<<(std::ostream& os, const CalcLabelTimingInfo& calcLabelTimingInfo) {
    os << "mergeTime: " << calcLabelTimingInfo.mergeTime << '\n';
    os << "lookUpTime: " << calcLabelTimingInfo.lookUpTime << '\n';
    os << "graphSearchTime: " << calcLabelTimingInfo.graphSearchTime << '\n';
    return os;
  }
};

struct RoutingResult {
  std::vector<uint32_t> edgeIds;
  std::vector<LatLng> path;
  std::vector<CellId_t> cellIds;
  Distance distance;
  double distanceTime;
  double pathTime = 0;
  double cellTime = 0;
  double nodeSearchTime = 0;
  CalcLabelTimingInfo calcLabelTimingInfo{};
};
// used for dijkstra PQ
struct CostNode {
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
using costNodeVec_t = std::vector<CostNode>;

}
