//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHGRAPH_H
#define ALG_ENG_PROJECT_CHGRAPH_H

#include "Graph.h"
#include <algorithm>
#include <map>

namespace pathFinder {
struct CHNode : Node {
  Level level;
};
template <template <class, class> class Vector = std::vector,
          class OffsetVector = std::vector<NodeId>>
class CHGraph {
  using EdgeVector = Vector<Edge, std::allocator<Edge>>;
  using NodeVector = Vector<CHNode, std::allocator<CHNode>>;

private:
  NodeVector nodes;
  EdgeVector backEdges;
  OffsetVector backOffset;

public:
  CHGraph(NodeVector &nodes, EdgeVector &edges, EdgeVector &backEdges,
          OffsetVector &offset, OffsetVector &backOffset, size_t numberOfNodes);

  CHGraph();

  OffsetVector offset;
  EdgeVector edges;
  size_t numberOfNodes;
  std::map<std::pair<Lat, Lng>, std::pair<NodeId, NodeId>> gridMap;
  [[nodiscard]] MyIterator<const Edge *> edgesFor(NodeId node,
                                                  EdgeDirection direction) {
    switch (direction) {
    case FORWARD:
      return {&edges[offset[node]], &edges[offset[node + 1]]};
    case BACKWARD:
      return {&backEdges[backOffset[node]], &backEdges[backOffset[node + 1]]};
    }
    return {&edges[offset[node]], &edges[offset[node + 1]]};
  }
  Level getLevel(NodeId nodeId) { return nodes[nodeId].level; }
  void sortByLevel(std::vector<CHNode> &sortedNodes);
  void sortEdges();
  NodeVector &getNodes();
  EdgeVector &getForwardEdges();
  OffsetVector &getForwardOffset();
  EdgeVector &getBackEdges();
  OffsetVector &getBackOffset();
  void deleteNodes();
  void deleteEdges();
};
template <template <class, class> class Vector, class OffsetVector>
void pathFinder::CHGraph<Vector, OffsetVector>::sortByLevel(
    std::vector<CHNode> &sortedNodes) {
  sortedNodes.reserve(nodes.size());
  for (const auto &node : nodes)
    sortedNodes.emplace_back(node);
  std::sort(sortedNodes.begin(), sortedNodes.end(),
            [](const auto &node1, const auto &node2) -> bool {
              return node1.level == node2.level ? node1.id < node2.id
                                                : node1.level > node2.level;
            });
}
template <template <class, class> class Vector, class OffsetVector>
Vector<CHNode, std::allocator<CHNode>> &
pathFinder::CHGraph<Vector, OffsetVector>::getNodes() {
  return nodes;
}
template <template <class, class> class Vector, class OffsetVector>
Vector<Edge, std::allocator<Edge>> &
pathFinder::CHGraph<Vector, OffsetVector>::getBackEdges() {
  return backEdges;
}
template <template <class, class> class Vector, class OffsetVector>
OffsetVector &pathFinder::CHGraph<Vector, OffsetVector>::getBackOffset() {
  return backOffset;
}
template <template <class, class> class Vector, class OffsetVector>
Vector<Edge, std::allocator<Edge>> &
pathFinder::CHGraph<Vector, OffsetVector>::getForwardEdges() {
  return edges;
}
template <template <class, class> class Vector, class OffsetVector>
OffsetVector &pathFinder::CHGraph<Vector, OffsetVector>::getForwardOffset() {
  return offset;
}

template <template <class, class> class Vector, class OffsetVector>
CHGraph<Vector, OffsetVector>::CHGraph(NodeVector &nodes, EdgeVector &edges,
                                       EdgeVector &backEdges,
                                       OffsetVector &offset,
                                       OffsetVector &backOffset,
                                       size_t numberOfNodes)
    : nodes(nodes), backEdges(backEdges), backOffset(backOffset),
      offset(offset), edges(edges), numberOfNodes(numberOfNodes) {}

template <template <class, class> class Vector, class OffsetVector>
CHGraph<Vector, OffsetVector>::CHGraph() {}

template <template <class, class> class Vector, class OffsetVector>
void CHGraph<Vector, OffsetVector>::deleteNodes() {
  nodes.clear();
  nodes.shrink_to_fit();
}

template <template <class, class> class Vector, class OffsetVector>
void CHGraph<Vector, OffsetVector>::deleteEdges() {
  edges.clear();
  edges.shrink_to_fit();
}
template <template <class, class> class Vector, class OffsetVector>
void CHGraph<Vector, OffsetVector>::sortEdges() {
  std::sort(edges.begin(), edges.end(), [](auto edge1, auto edge2) -> bool{
    return (edge1.source == edge2.source) ? edge1.target <= edge2.target : edge1.source < edge2.source;
  });
}
} // namespace pathFinder
#endif // ALG_ENG_PROJECT_CHGRAPH_H
