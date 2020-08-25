#ifndef ALG_ENG_PROJECT_CHGRAPH_H
#define ALG_ENG_PROJECT_CHGRAPH_H

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
  CHNode* nodes = nullptr;
  CHEdge* edges = nullptr;
  CHEdge* backEdges = nullptr;
  NodeId* offset = nullptr;
  NodeId* backOffset = nullptr;
  size_t numberOfNodes = 0;
  size_t numberOfEdges = 0;
  bool nodesMMap = false;
  bool edgesMMap = false;
  bool backEdgesMMap = false;
  bool offsetMMap = false;
  bool backOffsetMMap = false;
  bool gridCalculated = false;
  std::shared_ptr<Grid> grid;

  void setAllMMap(bool condition) {
    nodesMMap = condition;
    edgesMMap = condition;
    backEdgesMMap = condition;
    offsetMMap = condition;
    backOffsetMMap = condition;
  }
};
class CHGraph {
  /*
friend class FileReader;
*/
friend class GraphReader;
friend class FileWriter;
public:
  CHGraph();

  ~CHGraph();

  explicit CHGraph(CHGraphCreateInfo chGraphCreateInfo);

  static std::shared_ptr<CHGraph> makeShared(CHGraphCreateInfo chGraphCreateInfo);

  std::shared_ptr<Grid> grid;

  [[nodiscard]] MyIterator<const CHEdge *> edgesFor(NodeId node, EdgeDirection direction) const;
  Level getLevel(NodeId nodeId);
  void sortByLevel(std::vector<CHNode> &sortedNodes);
  void sortEdges();
  void randomizeLatLngs();
  NodeId getNodeIdFor(LatLng latLng);
  [[nodiscard]] CHNode getNode(NodeId id) const;
  std::optional<size_t> getEdgePosition(const CHEdge& edge, EdgeDirection direction);
  std::vector<CHEdge> getPathFromShortcut(CHEdge shortcut, double minLength);
  double getDistance(NodeId node1, NodeId node2);
  static double beeLineWithoutSquareRoot(LatLng latLng1, LatLng latLng2);
  [[nodiscard]] bool isValidNodeId(NodeId id) const;
  size_t getNumberOfNodes() const;
  size_t getNumberOfEdges() const;
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

  MyIterator<CHNode* > getNodesMutable();
  MyIterator<CHEdge* > getEdgesMutable();
  MyIterator<CHEdge*> getBackEdgesMutable();

};
} // namespace pathFinder
#endif // ALG_ENG_PROJECT_CHGRAPH_H
