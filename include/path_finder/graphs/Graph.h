#pragma once

#include <path_finder/helper/Types.h>

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
class Graph {
public:
  typedef std::vector<Node> nodeVector;
  typedef std::vector<Edge> edgeVector;

private:
public:
  NodeId numberOfNodes{};
  std::vector<NodeId> offset;
  edgeVector edges;
  nodeVector nodes;
  Graph() = default;
  ~Graph() = default;
  friend auto operator<<(std::ostream &Str, Graph graph) -> std::ostream & {

    for (auto edge : graph.edges) {
      Str << edge << '\n';
    }
    for (int i = 0; i < graph.offset.size(); ++i) {
      Str << i << ":" << graph.offset[i] << std::endl;
    }
    Str << "nodes: " << graph.numberOfNodes << std::endl;
    Str << "edges: " << graph.edges.size() << std::endl;
    Str << "offset: " << graph.offset.size() << std::endl;
    return Str;
  }
  [[nodiscard]] auto edgesFor(NodeId node) const -> MyIterator<const Edge *> {
    return {&edges[offset[node]], &edges[offset[node + 1]]};
  }
  [[maybe_unused]] [[nodiscard]] auto getNodeId(LatLng latLng) const -> NodeId;
  [[maybe_unused]] [[nodiscard]] auto getLatLng(NodeId nodeId) const -> LatLng;
};
class PreviousReplacer {
private:
  NodeId currentNode;

public:
  explicit PreviousReplacer(NodeId id) { currentNode = id; }
  auto operator()(CostNode costNode) const -> CostNode {
    if (costNode.previousNode == costNode.id) {
      // the previous node of the label has not been set so replace it
      // with the current node
      return CostNode(costNode.id, costNode.cost, currentNode);
    }
    return costNode;
  }
  auto operator()(NodeId id, Distance cost, NodeId previousNode) -> CostNode {
    return (*this)(CostNode(id, cost, previousNode));
  }
};
} // namespace pathFinder
