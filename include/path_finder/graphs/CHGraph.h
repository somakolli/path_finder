//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_CHGRAPH_H
#define ALG_ENG_PROJECT_CHGRAPH_H

#include "Graph.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <stack>

namespace pathFinder {
struct CHNode : Node {
  Level level;
};
struct CHEdge : Edge {
  std::optional<NodeId> child1;
  std::optional<NodeId> child2;
};
template <template <class, class> class Vector = std::vector,
          class OffsetVector = std::vector<NodeId>>
class CHGraph {
  using EdgeVector = Vector<CHEdge, std::allocator<CHEdge>>;
  using NodeVector = Vector<CHNode, std::allocator<CHNode>>;

private:
  NodeVector m_nodes;
  EdgeVector m_backEdges;
  OffsetVector m_backOffset;

public:
  CHGraph(NodeVector &nodes, EdgeVector &edges, EdgeVector &backEdges,
          OffsetVector &offset, OffsetVector &backOffset, size_t numberOfNodes);

  CHGraph();

  static std::shared_ptr<CHGraph> makeShared(NodeVector &nodes, EdgeVector &edges, EdgeVector &backEdges,
                                      OffsetVector &offset, OffsetVector &backOffset, size_t numberOfNodes);

  OffsetVector m_offset;
  EdgeVector m_edges;
  size_t m_numberOfNodes;
  std::map<std::pair<Lat, Lng>, std::pair<NodeId, NodeId>> gridMap;
  [[nodiscard]] MyIterator<const CHEdge *> edgesFor(NodeId node,
                                                  EdgeDirection direction) {
    switch (direction) {
    case FORWARD:
      return {&m_edges[m_offset[node]], &m_edges[m_offset[node + 1]]};
    case BACKWARD:
      return {&m_backEdges[m_backOffset[node]], &m_backEdges[m_backOffset[node + 1]]};
    }
    return {&m_edges[m_offset[node]], &m_edges[m_offset[node + 1]]};
  }
  Level getLevel(NodeId nodeId) {
    return m_nodes[nodeId].level;
  }
  void sortByLevel(std::vector<CHNode> &sortedNodes);
  void sortEdges();
  NodeId getNodeIdFor(LatLng latLng);
  NodeVector &getNodes();
  CHNode getNode(NodeId id) const;
  EdgeVector &getForwardEdges();
  OffsetVector &getForwardOffset();
  EdgeVector &getBackEdges();
  OffsetVector &getBackOffset();
  std::optional<size_t> getEdgePosition(const CHEdge& edge, EdgeDirection direction);
  std::vector<CHEdge> getPathFromShortcut(CHEdge shortcut, double minLength);
  double getDistance(NodeId node1, NodeId node2);
  static double beeLineWithoutSquareRoot(LatLng latLng1, LatLng latLng2);
  void deleteNodes();
  void deleteEdges();
};
template <template <class, class> class Vector, class OffsetVector>
void pathFinder::CHGraph<Vector, OffsetVector>::sortByLevel(
    std::vector<CHNode> &sortedNodes) {
  sortedNodes.reserve(m_nodes.size());
  for (const auto &node : m_nodes)
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
  return m_nodes;
}
template <template <class, class> class Vector, class OffsetVector>
Vector<CHEdge, std::allocator<CHEdge>> &
pathFinder::CHGraph<Vector, OffsetVector>::getBackEdges() {
  return m_backEdges;
}
template <template <class, class> class Vector, class OffsetVector>
OffsetVector &pathFinder::CHGraph<Vector, OffsetVector>::getBackOffset() {
  return m_backOffset;
}
template <template <class, class> class Vector, class OffsetVector>
Vector<CHEdge, std::allocator<CHEdge>> &
pathFinder::CHGraph<Vector, OffsetVector>::getForwardEdges() {
  return m_edges;
}
template <template <class, class> class Vector, class OffsetVector>
OffsetVector &pathFinder::CHGraph<Vector, OffsetVector>::getForwardOffset() {
  return m_offset;
}

template <template <class, class> class Vector, class OffsetVector>
CHGraph<Vector, OffsetVector>::CHGraph(NodeVector &nodes, EdgeVector &edges,
                                       EdgeVector &backEdges,
                                       OffsetVector &offset,
                                       OffsetVector &backOffset,
                                       size_t numberOfNodes)
    : m_nodes(nodes), m_backEdges(backEdges), m_backOffset(backOffset),
      m_offset(offset), m_edges(edges), m_numberOfNodes(numberOfNodes) {}

template <template <class, class> class Vector, class OffsetVector>
CHGraph<Vector, OffsetVector>::CHGraph() {}

template <template <class, class> class Vector, class OffsetVector>
void CHGraph<Vector, OffsetVector>::deleteNodes() {
  m_nodes.clear();
  m_nodes.shrink_to_fit();
}

template <template <class, class> class Vector, class OffsetVector>
void CHGraph<Vector, OffsetVector>::deleteEdges() {
  m_edges.clear();
  m_edges.shrink_to_fit();
}
template <template <class, class> class Vector, class OffsetVector>
void CHGraph<Vector, OffsetVector>::sortEdges() {
  std::sort(m_edges.begin(), m_edges.end(), [](auto edge1, auto edge2) -> bool{
    return (edge1.source == edge2.source) ? edge1.target <= edge2.target : edge1.source < edge2.source;
  });
}
template <template <class, class> class Vector, class OffsetVector>
std::vector<CHEdge> CHGraph<Vector, OffsetVector>::getPathFromShortcut(CHEdge shortcut, double minLength) {
  std::vector<CHEdge> path;
  Node source = m_nodes[shortcut.source];
  Node target = m_nodes[shortcut.target];
  double length = source.euclid(target);

  if(!shortcut.child2.has_value() || length <= minLength) {
    //std::cout << "is not shortcut" << std::endl;
    path.push_back(shortcut);
    return path;
  }
  //std::cout << "found shortcut" << std::endl;
  std::stack<uint32_t> edgesStack;

  edgesStack.push(shortcut.child2.value());
  edgesStack.push(shortcut.child1.value());

  while (!edgesStack.empty()) {
    auto& edgeIdx = edgesStack.top();
    edgesStack.pop();
    const auto& edge = m_edges[edgeIdx];
    //std::cout << "edge from stack: " << edge << std::endl;
    //std::cout << "source: " << nodes[edge.source] << std::endl;
    //std::cout << "target: " << nodes[edge.target] << std::endl;
    double length = m_nodes[edge.source].euclid(m_nodes[edge.target]);
    if(edge.child1.has_value() && length > minLength) {
      //std::cout << "is shortcut" << std::endl;

      edgesStack.push(edge.child2.value());
      edgesStack.push(edge.child1.value());
    } else {
      //std::cout << "is not shortcut" << std::endl;
      path.push_back(edge);
    }
  }
  return path;
}
template <template <class, class> class Vector, class OffsetVector>
CHNode CHGraph<Vector, OffsetVector>::getNode(NodeId id) const {
  return m_nodes[id];
}
template <template <class, class> class Vector, class OffsetVector>
std::optional<size_t> CHGraph<Vector, OffsetVector>::getEdgePosition(const CHEdge& edge, EdgeDirection direction) {
  for(auto i = m_offset[edge.source]; i < m_offset[edge.source+1]; ++i) {
    auto e = m_edges[i];
    if(e.target == edge.target) {
      return i;
    }
  }
  return std::nullopt;
}
template <template <class, class> class Vector, class OffsetVector>
NodeId CHGraph<Vector, OffsetVector>::getNodeIdFor(LatLng latLng) {
  double distance = std::numeric_limits<double>::max();
  NodeId position = m_nodes.size();
  for(int i = 0; i < m_nodes.size(); ++i) {
    auto& node = m_nodes[i];
    double currentDistance = beeLineWithoutSquareRoot(node.latLng, latLng);
    if(currentDistance < distance){
      distance = currentDistance;
      position = i;
    }
  }
  std::cout << latLng.lat << latLng.lng << std::endl;
  std::cout << position << std::endl;
  return position;
}
template <template <class, class> class Vector, class OffsetVector>
double CHGraph<Vector, OffsetVector>::beeLineWithoutSquareRoot(LatLng latLng1,
                                                               LatLng latLng2) {
  return pow(latLng1.lat - latLng2.lat, 2) + pow(latLng1.lng - latLng2.lng, 2);
}
template <template <class, class> class Vector, class OffsetVector>
std::shared_ptr<CHGraph<Vector, OffsetVector>> CHGraph<Vector, OffsetVector>::makeShared(
    NodeVector &nodes, EdgeVector &edges, EdgeVector &backEdges,
    OffsetVector &offset, OffsetVector &backOffset, size_t numberOfNodes) {
  return std::make_shared<CHGraph<Vector, OffsetVector>>(CHGraph(
      nodes, edges, backEdges, offset, backOffset, numberOfNodes
      ));
}
} // namespace pathFinder
#endif // ALG_ENG_PROJECT_CHGRAPH_H
