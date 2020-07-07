#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-default-arguments"
//
// Created by sokol on 17.06.20.
//

#ifndef MASTER_ARBEIT_HYBRIDPATHFINDER_H
#define MASTER_ARBEIT_HYBRIDPATHFINDER_H
#include "CHGraph.h"
#include "CellIdStore.h"
#include "HubLabelCreator.h"
#include "HubLabelStore.h"
#include "PathFinderBase.h"
#include "Static.h"
#include <queue>
#include <vector>
namespace pathFinder {
/**
 * @brief
 * Hybrid path finder which combines hub labels and contraction hierarchies
 * to enable sub millisecond world scale shortest path queries.
 *
 * @tparam HubLabelStore
 * @tparam Graph
 * @tparam CellIdStore
 */
template <typename HubLabelStore = HubLabelStore<std::vector>,
    typename Graph = CHGraph<std::vector>,
    typename CellIdStore = CellIdStore<std::vector>>
class HybridPathFinder : public PathFinderBase {

private:
  typedef std::vector<CostNode> costNodeVec_t;

  HubLabelStore& m_hubLabelStore;
  Graph& m_graph;
  CellIdStore &m_cellIdStore;
  Level m_labelsUntilLevel = 0;
  std::vector<Distance> m_cost;
  std::vector<NodeId> m_visited;
  /**
   * @brief
   * Calculates the label for a given source node id and direction.
   * @details
   * Does a ch query search until all labels which need to be fetched are known
   * and merges those labels to dynamicaly generate a hub label.
   * If the label is already pre computed it just returns that label.
   * @param source node id of the node where the search starts
   * @param direction direction in which to search (either use the forward (normal) edges
   * or backward (reverse) edges)
   * @return returns a vector which contains the label elements [nodeId, cost, previousNode]
   */
  costNodeVec_t calcLabelHybrid(NodeId source, EdgeDirection direction);
  /**
   * @brief
   * returns edge path from a node id path
   * @param path node id path
   * @param direction
   * @return edge path vector
   */
  std::vector<CHEdge> getEdgeVectorFromNodeIdPath(const std::vector<NodeId>& path, EdgeDirection direction);
  /**
   * @brief
   * add oscar cell ids which correspond to the given edge
   * @param edge needs to be in forward order
   * @param result cell id vector in which the cell ids will be added
   */
  void addCellIds(const CHEdge &edge, std::vector<CellId_t> &result);
  /**
   * @brief
   * Traverses the previous node entries in the label and finds the path.
   * @param label
   * @param topNode
   * @param sourceId
   * @return vector with nodeId visited in the path
   */
  std::vector<NodeId> getShortestPathFromLabel(const costNodeVec_t& label, NodeId topNode, NodeId sourceId);
  /**
   * finds the element given an id in a label
   * @param nodeId
   * @param label
   * @return the found element
   */
  std::optional<CostNode> findElementInLabel(NodeId nodeId,
                                             const costNodeVec_t &label);
public:
  /**
   * @brief
   * Initialize stores, graph, level, cost array.
   * @param hubLabelStore store with computed hubLabels
   * @param graph contraction hierarchy graph
   * @param cellIdStore store for egde -> oscar cell id
   * @param labelsUntilLevel level until the labels are computed in the store
   */
  HybridPathFinder(HubLabelStore &hubLabelStore, Graph &graph,
                   CellIdStore &cellIdStore, Level labelsUntilLevel)
      : m_hubLabelStore(hubLabelStore), m_graph(graph),
        m_cellIdStore(cellIdStore), m_labelsUntilLevel(labelsUntilLevel) {
    m_cost.reserve(graph.getNodes().size());
    while (m_cost.size() < graph.getNodes().size())
      m_cost.push_back(MAX_DISTANCE);
  }
  /**
   * @brief
   * Computes the shortest path and distance from one node id to another.
   * @details
   * Does a contraction hierarchy query until the level for which the labels are
   * precomputed. Then merges those found labels to get the path and distance.
   * Also finds all the oscar cells which are visited while traversing the path.
   * @param source node id of the source node
   * @param target node id of the target node
   * @param distance the distance will be stored in this variable
   * @param cellIds the oscar cell ids of the path will be stored in this container
   * @return vector containing the path in latitude longitude coordinates
   */
  std::vector<LatLng> getShortestPath(NodeId source, NodeId target, std::optional<Distance>& distance,
                                      std::vector<CellId_t> *cellIds = nullptr) override;
  /**
   * @brief
   * Computes the shortest path and distance from one coordinate to another.
   * @details
   * Finds the corresponding node ids for the coordinates.
   * Does a contraction hierarchy query until the level for which the labels are
   * precomputed. Then merges those found labels to get the path and distance.
   * Also finds all the oscar cells which are visited while traversing the path.
   * @param source coordinates of the source node
   * @param target coordinates of the target node
   * @param distance the distance will be stored in this variable
   * @param cellIds the oscar cell ids of the path will be stored in this container
   * @return vector containing the path in latitude longitude coordinates
   */
  std::vector<LatLng> getShortestPath(LatLng source, LatLng target, std::optional<Distance>& distance, std::vector<CellId_t> *cellIds = nullptr) override;
};
template <typename HubLabelStore, typename Graph, typename CellIdStore>
std::vector<LatLng>
HybridPathFinder<HubLabelStore, Graph, CellIdStore>::getShortestPath(
    NodeId source, NodeId target, std::optional<Distance> &distance,
    std::vector<CellId_t> *cellIds) {
  auto forwardLabel = calcLabelHybrid(source, EdgeDirection::FORWARD);
  auto backwardLabel = calcLabelHybrid(target, EdgeDirection::BACKWARD);
  NodeId topNode;
  distance = Static::getShortestDistance(MyIterator(forwardLabel.begin().base(), forwardLabel.end().base()),
                                         MyIterator(backwardLabel.begin().base(), backwardLabel.end().base()),
                                         topNode);

  // the forward path is in reverse the backward path is correct
  auto reverseForwardPath = getShortestPathFromLabel(forwardLabel, topNode, source);
  auto backwardPath = getShortestPathFromLabel(backwardLabel, topNode, target);

  // reverse the forward path
  std::vector<NodeId> forwardPath;
  for(int i = reverseForwardPath.size() - 1; i >= 0; --i) {
    forwardPath.push_back(reverseForwardPath[i]);
  }

  // get edges to unpack them
  std::vector<CHEdge> forwardEdges = getEdgeVectorFromNodeIdPath(forwardPath, EdgeDirection::FORWARD);
  std::vector<CHEdge> backwardEdges = getEdgeVectorFromNodeIdPath(backwardPath, EdgeDirection::BACKWARD);

  // unpack edges
  std::vector<CHEdge> finalForwardEdgePath;
  for(auto edge : forwardEdges) {
    for (auto e : m_graph.getPathFromShortcut(edge, 0))
      finalForwardEdgePath.emplace_back(e);
  }

  std::vector<CHEdge> finalBackwardEdgePath;
  for(auto edge : backwardEdges) {
    for (auto e : m_graph.getPathFromShortcut(edge, 0))
      finalBackwardEdgePath.emplace_back(e);
  }
  // find cell ids
  if(cellIds != nullptr){
    // forward
    for(auto edge : finalForwardEdgePath){
      addCellIds(edge, *cellIds);
    }
    // backward
    for(auto edge : finalBackwardEdgePath){
      addCellIds(edge, *cellIds);
    }
  }
  // get lat longs
  std::vector<LatLng> latLngVector;
  if(!finalForwardEdgePath.empty())
    latLngVector.push_back(
        m_graph.getNodes()[finalForwardEdgePath[0].source].latLng);
  for(auto edge : finalForwardEdgePath) {
    latLngVector.push_back(m_graph.getNodes()[edge.target].latLng);
  }
  if(!finalBackwardEdgePath.empty())
    latLngVector.push_back(
        m_graph.getNodes()[finalBackwardEdgePath[0].target].latLng);
  for(auto edge : finalBackwardEdgePath) {
    latLngVector.push_back(m_graph.getNodes()[edge.source].latLng);
  }

  // remove duplicates from cellIds
  if(cellIds != nullptr){
    sort( (*cellIds).begin(), (*cellIds).end() );
    (*cellIds).erase(
        unique((*cellIds).begin(), (*cellIds).end()), (*cellIds).end()
    );
  }

  return latLngVector;
}
template <typename HubLabelStore, typename Graph, typename CellIdStore>
std::vector<LatLng>
HybridPathFinder<HubLabelStore, Graph, CellIdStore>::getShortestPath(
    LatLng source, LatLng target, std::optional<Distance> &distance,
    std::vector<CellId_t> *cellIds) {
  NodeId sourceId = m_graph.getNodeIdFor(source);
  NodeId targetId = m_graph.getNodeIdFor(target);
  return getShortestPath(sourceId, targetId, distance, cellIds);
}
template <typename HubLabelStore, typename Graph, typename CellIdStore>
std::vector<CostNode>
HybridPathFinder<HubLabelStore, Graph, CellIdStore>::calcLabelHybrid(
    NodeId source, EdgeDirection direction) {
  const auto &sourceLabel = m_hubLabelStore.retrieve(source, direction);
  if (m_graph.getLevel(source) >= m_labelsUntilLevel) {
    costNodeVec_t vec;
    for (const auto entry : sourceLabel)
      vec.push_back(entry);
    return vec;
  }
  std::vector<CostNode> settledNodes;
  std::map<NodeId, std::vector<NodeId>> labelsToCollectMap;
  for (auto nodeId : m_visited)
    m_cost[nodeId] = MAX_DISTANCE;
  m_visited.clear();
  std::priority_queue<CostNode> q;
  q.emplace(source, 0, source);
  m_cost[source] = 0;
  m_visited.emplace_back(source);
  // ch dijkstra
  while (!q.empty()) {
    auto costNode = q.top();
    q.pop();
    if (costNode.cost > m_cost[costNode.id])
      continue;
    settledNodes.emplace_back(costNode.id, costNode.cost,
                              costNode.previousNode);
    auto currentNode = m_graph.getNodes()[costNode.id];
    if (currentNode.level >= m_labelsUntilLevel) {
      if (labelsToCollectMap.find(costNode.previousNode) ==
          labelsToCollectMap.end()) {
        labelsToCollectMap[costNode.previousNode] = std::vector<NodeId>();
        labelsToCollectMap[costNode.previousNode].emplace_back(costNode.id);
      } else
        labelsToCollectMap[costNode.previousNode].emplace_back(costNode.id);
      continue;
    }
    for (const auto &edge : m_graph.edgesFor(costNode.id, direction)) {
      if (m_graph.getLevel(edge.source) > m_graph.getLevel(edge.target))
        continue;
      auto addedCost = costNode.cost + edge.distance;
      if (addedCost < m_cost[edge.target]) {
        m_visited.emplace_back(edge.target);
        m_cost[edge.target] = addedCost;
        q.emplace(edge.target, addedCost, edge.source);
      }
    }
  }
  Static::sortLabel(settledNodes);
  for (auto [previousNodeId, labelsToCollect] : labelsToCollectMap) {
    for (auto id : labelsToCollect) {
      costNodeVec_t targetLabel;
      for (const auto &entry : m_hubLabelStore.retrieve(id, direction))
        targetLabel.push_back(entry);
      costNodeVec_t resultVec;
      resultVec.reserve(settledNodes.size() + targetLabel.size());
      Static::merge(settledNodes.begin(), settledNodes.end(),
                    targetLabel.begin(), targetLabel.end(), m_cost[id],
                    resultVec, PreviousReplacer(previousNodeId));
      settledNodes = std::move(resultVec);
    }
  }
  return settledNodes;
}
template <typename HubLabelStore, typename Graph, typename CellIdStore>
std::vector<CHEdge> HybridPathFinder<HubLabelStore, Graph, CellIdStore>::
    getEdgeVectorFromNodeIdPath(const std::vector<NodeId> &path,
                                EdgeDirection direction) {
  std::vector<CHEdge> edgePath;
  for(int i = 0; i < path.size() - 1; ++i) {
    for(CHEdge edge : m_graph.edgesFor(path[i], direction)) {
      if(edge.target == path[i+1])
        edgePath.emplace_back(edge);
    }
  }
  return edgePath;
}
template <typename HubLabelStore, typename Graph, typename CellIdStore>
void HybridPathFinder<HubLabelStore, Graph, CellIdStore>::addCellIds(
    const CHEdge &edge, std::vector<CellId_t> &result) {
  // edge always needs to be in forward order
  auto edgeId = m_graph.getEdgePosition(edge, EdgeDirection::FORWARD);
  if(!edgeId.has_value())
    throw std::runtime_error("[Hublabels.addCellIds]could not find edgeId for edge(" + std::to_string(edge.source) + "," + std::to_string(edge.target) + ")");
  for(auto cellId : m_cellIdStore.getCellIds(edgeId.value())){
    result.push_back(cellId);
  }
}
template <typename HubLabelStore, typename Graph, typename CellIdStore>
std::vector<NodeId>
HybridPathFinder<HubLabelStore, Graph, CellIdStore>::getShortestPathFromLabel(
    const costNodeVec_t &label, NodeId topNodeId, NodeId sourceId) {
  std::vector<NodeId> path;
  std::optional<CostNode> topNode = findElementInLabel(topNodeId, label);
  if(!topNode.has_value())
    throw std::runtime_error("could not find topNode in path");
  path.push_back(topNodeId);
  auto currentNode = topNode;
  while(currentNode.value().id != sourceId) {
    currentNode = findElementInLabel(currentNode.value().previousNode, label);
    if(!currentNode.has_value())
      throw std::runtime_error("could not find currentNode in path");
    path.push_back(currentNode.value().id);
  }
  return path;
}
template <typename HubLabelStore, typename Graph, typename CellIdStore>
std::optional<CostNode>
HybridPathFinder<HubLabelStore, Graph, CellIdStore>::findElementInLabel(
    NodeId nodeId, const costNodeVec_t &label) {
  // TODO: binary search here with std::lower_bound
  for(auto l : label) {
    if(l.id == nodeId)
      return l;
  }
  return std::nullopt;
}
} // namespace pathFinder
#endif // MASTER_ARBEIT_HYBRIDPATHFINDER_H

#pragma clang diagnostic pop
