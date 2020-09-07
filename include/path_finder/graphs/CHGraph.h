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

  [[maybe_unused]] static auto makeShared(const CHGraphCreateInfo &chGraphCreateInfo) -> std::shared_ptr<CHGraph>;

  std::shared_ptr<Grid> grid;
  BoundingBox boundingBox;
  LatLng midPoint{};

  [[nodiscard]] auto getLevel(NodeId nodeId) const -> Level;
  void sortByLevel(std::vector<CHNode> &sortedNodes);
  void sortEdges();
  void randomizeLatLngs();
  [[nodiscard]] auto getNodeIdFor(LatLng latLng, LatLng midPoint = LatLng(0,0)) const -> NodeId;

  [[nodiscard]] auto edgesFor(NodeId node, EdgeDirection direction) const -> MyIterator<const CHEdge *>;
  [[nodiscard]] auto getNode(NodeId id) const -> CHNode;
  [[nodiscard]] auto getEdgePosition(const CHEdge &edge, EdgeDirection direction) const -> std::optional<size_t>;
  [[nodiscard]] auto getPathFromShortcut(CHEdge shortcut, double minLength) const -> std::vector<CHEdge>;
  [[nodiscard]] auto isValidNodeId(NodeId id) const -> bool;
  [[nodiscard]] auto getNumberOfNodes() const -> size_t;
  [[nodiscard]] auto getNumberOfEdges() const -> size_t;
  [[nodiscard]] auto getNodes() const -> MyIterator<const CHNode *>;
  [[nodiscard]] auto getEdges() const -> MyIterator<const CHEdge *>;
  [[nodiscard]] auto getBackEdges() const -> MyIterator<const CHEdge *>;
  [[nodiscard]] auto getForwardOffset() const -> MyIterator<const NodeId *>;
  [[nodiscard]] auto getBackwardOffset() const -> MyIterator<const NodeId *>;

  static auto beeLineWithoutSquareRoot(LatLng latLng1, LatLng latLng2) -> double;

private:
  CHNode *m_nodes = nullptr;
  CHEdge *m_edges = nullptr;
  CHEdge *m_backEdges = nullptr;
  NodeId *m_offset = nullptr;
  NodeId *m_backOffset = nullptr;
  size_t m_numberOfEdges{};
  size_t m_numberOfNodes{};

  bool m_nodesMMap = false;
  bool m_edgesMMap = false;
  bool m_backEdgesMMap = false;
  bool m_offsetMMap = false;
  bool m_backOffsetMMap = false;

  auto getNodesMutable() -> MyIterator<CHNode *>;
  auto getEdgesMutable() -> MyIterator<CHEdge *>;
  auto getBackEdgesMutable() -> MyIterator<CHEdge *>;
};
} // namespace pathFinder
