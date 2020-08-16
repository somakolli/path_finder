#ifndef ALG_ENG_PROJECT_CHGRAPH_H
#define ALG_ENG_PROJECT_CHGRAPH_H

#include "Graph.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <stack>
#include <sys/mman.h>

namespace pathFinder {
struct CHNode : Node {
  Level level;
};
struct CHEdge : Edge {
  std::optional<NodeId> child1;
  std::optional<NodeId> child2;
};
struct CHGraphCreateInfo {
  CHNode* nodes;
  CHEdge* edges;
  CHEdge* backEdges;
  NodeId* offset;
  NodeId* backOffset;
  size_t numberOfNodes;
  size_t numberOfEdges;
  bool nodesMMap = false;
  bool edgesMMap = false;
  bool backEdgesMMap = false;
  bool offsetMMap = false;
  bool backOffsetMMap = false;

  void setAllMMap() {
    nodesMMap = true;
    edgesMMap = true;
    backEdgesMMap = true;
    offsetMMap = true;
    backOffsetMMap = true;
  }
};
class CHGraph {
  /*
friend class FileReader;
*/
friend class GraphReader;
friend class FileWriter;
friend class Grid;
FRIEND_TEST(Graph, ReadWorks);
FRIEND_TEST(Graph, GridReorderWorks);
FRIEND_TEST(Graph, ReadFromDiskWorks);
public:
  CHGraph();

  ~CHGraph();

  CHGraph(CHGraphCreateInfo chGraphCreateInfo);

  static std::shared_ptr<CHGraph> makeShared(CHGraphCreateInfo chGraphCreateInfo);

  std::map<std::pair<Lat, Lng>, std::pair<NodeId, NodeId>> gridMap;

  [[nodiscard]] MyIterator<const CHEdge *> edgesFor(NodeId node, EdgeDirection direction);
  Level getLevel(NodeId nodeId);
  void sortByLevel(std::vector<CHNode> &sortedNodes);
  void sortEdges();
  void randomizeLatLngs();
  NodeId getNodeIdFor(LatLng latLng);
  CHNode getNode(NodeId id) const;
  std::optional<size_t> getEdgePosition(const CHEdge& edge, EdgeDirection direction);
  std::vector<CHEdge> getPathFromShortcut(CHEdge shortcut, double minLength);
  double getDistance(NodeId node1, NodeId node2);
  static double beeLineWithoutSquareRoot(LatLng latLng1, LatLng latLng2);
  bool isValidNodeId(NodeId id) const;
  size_t getNumberOfNodes();
  size_t getNumberOfEdges();
  MyIterator<const CHNode*> getNodes();
  MyIterator<const CHEdge*> getEdges();
  MyIterator<const CHEdge*> getBackEdges();
  MyIterator<const NodeId*> getForwardOffset();
  MyIterator<const NodeId*> getBackwardOffset();
private:
  CHNode* m_nodes = nullptr;
  CHEdge* m_edges = nullptr;
  CHEdge* m_backEdges = nullptr;
  NodeId* m_offset = nullptr;
  NodeId* m_backOffset = nullptr;
  size_t m_numberOfEdges;
  size_t m_numberOfNodes;

  bool m_nodesMMap = false;
  bool m_edgesMMap = false;
  bool m_backEdgesMMap = false;
  bool m_offsetMMap = false;
  bool m_backOffsetMMap = false;

};
} // namespace pathFinder
#endif // ALG_ENG_PROJECT_CHGRAPH_H
