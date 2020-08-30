#pragma once

#include "Graph.h"
#include "Grid.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <stack>
#include <sys/mman.h>

namespace pathFinder {
struct CHGraphCreateInfo {
  CHNode *nodes = nullptr;
  CHEdge *edges = nullptr;
  CHEdge *backEdges = nullptr;
  NodeId *offset = nullptr;
  NodeId *backOffset = nullptr;
  size_t numberOfNodes = 0;
  size_t numberOfEdges = 0;
  bool nodesMMap = false;
  bool edgesMMap = false;
  bool backEdgesMMap = false;
  bool offsetMMap = false;
  bool backOffsetMMap = false;
  bool gridCalculated;
  std::shared_ptr<Grid> grid;
  BoundingBox boundingBox;
  LatLng midPoint;
};
class CHGraph {
  friend class FileWriter;
  friend class GraphReader;

public:
  CHGraph();

  ~CHGraph();

  explicit CHGraph(CHGraphCreateInfo chGraphCreateInfo);

  [[maybe_unused]] static std::shared_ptr<CHGraph> makeShared(const CHGraphCreateInfo &chGraphCreateInfo);

  std::shared_ptr<Grid> grid;
  BoundingBox boundingBox;
  LatLng midPoint;

  [[nodiscard]] Level getLevel(NodeId nodeId) const;
  void sortByLevel(std::vector<CHNode> &sortedNodes);
  void sortEdges();
  void randomizeLatLngs();
  [[nodiscard]] NodeId getNodeIdFor(LatLng latLng) const;

  [[nodiscard]] MyIterator<const CHEdge *> edgesFor(NodeId node, EdgeDirection direction) const;
  [[nodiscard]] CHNode getNode(NodeId id) const;
  [[nodiscard]] std::optional<size_t> getEdgePosition(const CHEdge &edge, EdgeDirection direction) const;
  [[nodiscard]] std::vector<CHEdge> getPathFromShortcut(CHEdge shortcut, double minLength) const;
  [[nodiscard]] bool isValidNodeId(NodeId id) const;
  [[nodiscard]] size_t getNumberOfNodes() const;
  [[nodiscard]] size_t getNumberOfEdges() const;
  [[nodiscard]] MyIterator<const CHNode *> getNodes() const;
  [[nodiscard]] MyIterator<const CHEdge *> getEdges() const;
  [[nodiscard]] MyIterator<const CHEdge *> getBackEdges() const;
  [[nodiscard]] MyIterator<const NodeId *> getForwardOffset() const;
  [[nodiscard]] MyIterator<const NodeId *> getBackwardOffset() const;

  static double beeLineWithoutSquareRoot(LatLng latLng1, LatLng latLng2);

private:
  CHNode *m_nodes = nullptr;
  CHEdge *m_edges = nullptr;
  CHEdge *m_backEdges = nullptr;
  NodeId *m_offset = nullptr;
  NodeId *m_backOffset = nullptr;
  size_t m_numberOfEdges;
  size_t m_numberOfNodes;

  bool m_nodesMMap = false;
  bool m_edgesMMap = false;
  bool m_backEdgesMMap = false;
  bool m_offsetMMap = false;
  bool m_backOffsetMMap = false;

  MyIterator<CHNode *> getNodesMutable();
  MyIterator<CHEdge *> getEdgesMutable();
  MyIterator<CHEdge *> getBackEdgesMutable();
};
} // namespace pathFinder
