#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <path_finder/graphs/GeometricType.h>
#include <vector>

namespace pathFinder {
using NodeId = uint32_t;
using EdgeId = uint32_t;

using Distance = uint32_t;
using Level = uint16_t;
using CellId_t = uint32_t;
constexpr Distance MAX_DISTANCE = std::numeric_limits<Distance>::max();
enum EdgeDirection { FORWARD = true, BACKWARD = false };
template <typename MyPointerType>
class MyIterator {
private:
  MyPointerType _begin;
  MyPointerType _end;

public:
  MyIterator(MyPointerType begin, MyPointerType end) : _begin(begin), _end(end) {}
  [[nodiscard]] bool empty() const { return _begin == _end; };
  MyPointerType begin() { return _begin; };
  MyPointerType begin() const { return _begin; };
  MyPointerType end() { return _end; };
  MyPointerType end() const { return _end; };
  size_t size() { return _end - _begin; }
  auto operator[](size_t position) const { return *(begin() + position); }
};

struct OffsetElement {
  size_t position = 0;
  size_t size = 0;
};

void to_json(nlohmann::json &j, LatLng latLng);
void from_json(const nlohmann::json &j, LatLng &latLng);
class Node {
public:
  NodeId id{std::numeric_limits<NodeId>::max()};
  LatLng latLng;
  [[nodiscard]] auto quickBeeLine(const LatLng &other) const -> double;
  [[nodiscard]] auto quickBeeLine(const Node &other) const -> double;
  [[nodiscard]] auto euclid(const Node &other) const -> double;
};
class Edge {
public:
  NodeId source{std::numeric_limits<NodeId>::max()};
  NodeId target{std::numeric_limits<NodeId>::max()};
  NodeId distance{std::numeric_limits<NodeId>::max()};
  Edge(NodeId source, NodeId target, Distance distance) : source(source), target(target), distance(distance) {}
  Edge() = default;
  friend auto operator<<(std::ostream &Str, const Edge &edge) -> std::ostream & {
    Str << "source: " << edge.source << ' ' << "target: " << edge.target << " distance: " << edge.distance;
    return Str;
  }
};
struct CHNode : Node {
  Level level{std::numeric_limits<Level>::max()};
  auto toNode() -> Node { return Node{id, latLng}; }
};
struct CHEdge : Edge {
  std::optional<NodeId> child1 = std::nullopt;
  std::optional<NodeId> child2 = std::nullopt;
};

struct CalcLabelTimingInfo {
  double mergeTime = 0;
  double graphSearchTime = 0;
  double lookUpTime = 0;

  void operator+=(const CalcLabelTimingInfo &other) {
    mergeTime += other.mergeTime;
    graphSearchTime += other.graphSearchTime;
    lookUpTime += other.lookUpTime;
  }
  void operator/=(double other) {
    mergeTime /= other;
    lookUpTime /= other;
    graphSearchTime /= other;
  }
  auto operator/(double other) const -> CalcLabelTimingInfo {
    CalcLabelTimingInfo c{};
    c.mergeTime = this->mergeTime / other;
    c.lookUpTime = this->lookUpTime / other;
    c.graphSearchTime = this->graphSearchTime / other;
    return c;
  }
  friend auto operator<<(std::ostream &os, const CalcLabelTimingInfo &calcLabelTimingInfo) -> std::ostream & {
    os << "mergeTime: " << calcLabelTimingInfo.mergeTime << '\n';
    os << "lookUpTime: " << calcLabelTimingInfo.lookUpTime << '\n';
    os << "graphSearchTime: " << calcLabelTimingInfo.graphSearchTime << '\n';
    return os;
  }
};

struct RoutingResultTimingInfo {
  double distanceTime = 0;
  double pathTime = 0;
  double cellTime = 0;
  double nodeSearchTime = 0;
  CalcLabelTimingInfo calcLabelTimingInfo{};
  // operator to calculate averages
  auto operator/(uint32_t numberOfQueries) const -> RoutingResultTimingInfo {
    RoutingResultTimingInfo r{};
    r.distanceTime = this->distanceTime / numberOfQueries;
    r.nodeSearchTime = this->nodeSearchTime / numberOfQueries;
    r.cellTime = this->cellTime / numberOfQueries;
    r.pathTime = this->pathTime / numberOfQueries;
    r.calcLabelTimingInfo = this->calcLabelTimingInfo / numberOfQueries;
    return r;
  }
  void operator+=(RoutingResultTimingInfo other) {
    distanceTime += other.distanceTime;
    pathTime += other.pathTime;
    cellTime += other.cellTime;
    nodeSearchTime += other.nodeSearchTime;
    calcLabelTimingInfo += other.calcLabelTimingInfo;
  }
  [[nodiscard]] auto toJson() const -> std::string;
};
struct RoutingResult {
  std::vector<uint32_t> edgeIds;
  std::vector<uint32_t> shortcutEdgeIds;
  std::vector<LatLng> path;
  std::vector<CellId_t> cellIds;
  Distance distance;
  RoutingResultTimingInfo routingResultTimingInfo;
  void operator+=(const RoutingResult& other) {
    distance += other.distance;
    routingResultTimingInfo += other.routingResultTimingInfo;
    path.insert(path.end(), other.path.begin(), other.path.end());
    cellIds.insert(cellIds.end(), other.cellIds.begin(), other.cellIds.end());
  }
};
// used for dijkstra PQ
struct CostNode {
  NodeId id;
  Distance cost;
  NodeId previousNode;
  CostNode() = default;

  auto operator<(const CostNode &rhs) const -> bool { return cost > rhs.cost; }
  CostNode(NodeId id, size_t cost, NodeId previousNode) : id(id), cost(cost), previousNode(previousNode) {}

  auto operator==(const CostNode &rhs) const -> bool {
    return id == rhs.id && cost == rhs.cost && previousNode == rhs.previousNode;
  }
};
using costNodeVec_t = std::vector<CostNode>;

void to_json(nlohmann::json &j, CalcLabelTimingInfo calcLabelTimingInfo);
void to_json(nlohmann::json &j, RoutingResultTimingInfo calcLabelTimingInfo);

} // namespace pathFinder
