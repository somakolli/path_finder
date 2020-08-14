#pragma clang diagnostic push
#pragma ide diagnostic ignored "google-default-arguments"
//
// Created by sokol on 17.06.20.
//

#ifndef MASTER_ARBEIT_HYBRIDPATHFINDER_H
#define MASTER_ARBEIT_HYBRIDPATHFINDER_H

#include "PathFinderBase.h"
#include "path_finder/graphs/CHGraph.h"
#include "path_finder/helper/Static.h"
#include "path_finder/routing/HubLabelCreator.h"
#include "path_finder/storage/CellIdStore.h"
#include "path_finder/storage/HubLabelStore.h"
#include <path_finder/helper/Stopwatch.h>
#include <queue>
#include <utility>
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
template <typename Graph = CHGraph<std::vector>,
          typename CellIdStore = CellIdStore<CellId_t>>
class HybridPathFinder : public PathFinderBase {
public:
  /**
   * @brief
   * Initialize stores, graph, level, cost array.
   * @param hubLabelStore store with computed hubLabels
   * @param graph contraction hierarchy graph
   * @param cellIdStore store for egde -> oscar cell id
   * @param labelsUntilLevel level until the labels are computed in the store
   */
  HybridPathFinder(std::shared_ptr<HubLabelStore> hubLabelStore, std::shared_ptr<Graph> graph,
                   std::shared_ptr<CellIdStore> cellIdStore, Level labelsUntilLevel,
                   bool hubLabelsCalculated = false, bool cellIdsCalculated = false)
      : m_hubLabelStore(std::move(hubLabelStore)), m_graph(graph),
        m_cellIdStore(cellIdStore), m_labelsUntilLevel(labelsUntilLevel),
        m_hubLabelsCalculated(hubLabelsCalculated),
        m_cellIdsCalculated(cellIdsCalculated){
    m_cost.reserve(graph->getNodes().size());
    while (m_cost.size() < graph->getNodes().size())
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
   * @return RoutingResult object with path, distance, cell ids
   */
  RoutingResult getShortestPath(NodeId source, NodeId target) override;

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
   * @param cellIds the oscar cell ids of the path will be stored in this
   * container
   * @return RoutingResult object with path, distance, cell ids
   */
  RoutingResult getShortestPath(LatLng source, LatLng target) override;
  size_t graphNodeSize();
  Level labelsUntilLevel();
  auto getGraph();
  Level getMaxLevel();
  void setLabelsUntilLevel(Level level);
private:
  typedef std::vector<CostNode> costNodeVec_t;

  std::shared_ptr<HubLabelStore> m_hubLabelStore;
  std::shared_ptr<Graph> m_graph;
  std::shared_ptr<CellIdStore> m_cellIdStore;
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
   * @param direction direction in which to search (either use the forward
   * (normal) edges or backward (reverse) edges)
   * @return returns a vector which contains the label elements [nodeId, cost,
   * previousNode]
   */
  costNodeVec_t calcLabelHybrid(NodeId source, EdgeDirection direction, CalcLabelTimingInfo& calcLabelTimingInfo);

  /**
   * @brief
   * returns edge path from a node id path
   * @param path node id path
   * @param direction
   * @return edge path vector
   */
  std::vector<CHEdge>
  getEdgeVectorFromNodeIdPath(const std::vector<NodeId> &path,
                              EdgeDirection direction);

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
  std::vector<NodeId> getShortestPathFromLabel(const costNodeVec_t &label,
                                               NodeId topNode, NodeId sourceId);

  /**
   * finds the element given an id in a label
   * @param nodeId
   * @param label
   * @return the found element
   */
  std::optional<CostNode> findElementInLabel(NodeId nodeId,
                                             const costNodeVec_t &label);

  bool m_cellIdsCalculated = false;
  bool m_hubLabelsCalculated = false;
};

template <typename Graph, typename CellIdStore>
RoutingResult
HybridPathFinder<Graph, CellIdStore>::getShortestPath(
    NodeId source, NodeId target) {
  RoutingResult routingResult;
  Stopwatch stopwatch;
  auto forwardLabel = calcLabelHybrid(source, EdgeDirection::FORWARD, routingResult.calcLabelTimingInfo);
  auto backwardLabel = calcLabelHybrid(target, EdgeDirection::BACKWARD, routingResult.calcLabelTimingInfo);
  NodeId topNode;
  routingResult.distance = Static::getShortestDistance(
      MyIterator(forwardLabel.begin().base(), forwardLabel.end().base()),
      MyIterator(backwardLabel.begin().base(), backwardLabel.end().base()),
      topNode).value();
  routingResult.distanceTime = stopwatch.elapsedMicro();

  stopwatch.reset();
  // the forward path is in reverse the backward path is correct
  auto reverseForwardPath =
      getShortestPathFromLabel(forwardLabel, topNode, source);
  auto backwardPath = getShortestPathFromLabel(backwardLabel, topNode, target);

  // reverse the forward path
  std::vector<NodeId> forwardPath;
  for (int i = reverseForwardPath.size() - 1; i >= 0; --i) {
    forwardPath.push_back(reverseForwardPath[i]);
  }
  for(int i = 1; i < backwardPath.size(); ++i)
    forwardPath.push_back(backwardPath[i]);

  std::vector<CHEdge> newForwardEdges =
      getEdgeVectorFromNodeIdPath(forwardPath, EdgeDirection::FORWARD);
  // unpack edges
  std::vector<CHEdge> newFinalForwardEdgePath;
  for (auto edge : newForwardEdges) {
    for (auto e : m_graph->getPathFromShortcut(edge, 0))
      newFinalForwardEdgePath.emplace_back(e);
  }

  for(auto e : newFinalForwardEdgePath) {
    routingResult.edgeIds.emplace_back(m_graph->getEdgePosition(e, EdgeDirection::FORWARD).value());
  }

  // get lat longs
  std::vector<LatLng> newLatLngVector;
  if (!newFinalForwardEdgePath.empty())
    newLatLngVector.push_back(
        m_graph->getNodes()[newFinalForwardEdgePath[0].source].latLng);
  for (auto edge : newFinalForwardEdgePath) {
    newLatLngVector.push_back(m_graph->getNodes()[edge.target].latLng);
  }
  routingResult.path = newLatLngVector;
  routingResult.pathTime = stopwatch.elapsedMicro();

  stopwatch.reset();
  // find cell ids
  if(m_cellIdsCalculated)
    for (auto edge : newFinalForwardEdgePath) {
      addCellIds(edge, routingResult.cellIds);
    }
  // remove duplicates from cellIds
  sort(routingResult.cellIds.begin(), routingResult.cellIds.end());
  (routingResult.cellIds)
      .erase(unique(routingResult.cellIds.begin(), routingResult.cellIds.end()),
             routingResult.cellIds.end());
  routingResult.cellTime = stopwatch.elapsedMicro();
  return routingResult;
}

template <typename Graph, typename CellIdStore>
RoutingResult
HybridPathFinder<Graph, CellIdStore>::getShortestPath(
    LatLng source, LatLng target) {
  Stopwatch stopwatch;
  NodeId sourceId = m_graph->getNodeIdFor(source);
  NodeId targetId = m_graph->getNodeIdFor(target);
  auto nodeSearchTime = stopwatch.elapsedMicro();
  RoutingResult routingResult = getShortestPath(sourceId, targetId);
  routingResult.nodeSearchTime = nodeSearchTime;
  return routingResult;
}

template <typename Graph, typename CellIdStore>
std::vector<CostNode>
HybridPathFinder<Graph, CellIdStore>::calcLabelHybrid(
    NodeId source, EdgeDirection direction, CalcLabelTimingInfo& calcLabelTimingInfo) {
  Stopwatch stopwatch;
  if (m_graph->getLevel(source) >= m_labelsUntilLevel && m_hubLabelsCalculated) {
    CostNode* sourceLabel;
    size_t size;
    m_hubLabelStore->retrieve(source, direction, sourceLabel, size);
    costNodeVec_t vec;
    for (auto i = sourceLabel; sourceLabel < sourceLabel + size; ++sourceLabel)
      vec.push_back(*i);
    calcLabelTimingInfo.lookUpTime = stopwatch.elapsedMicro();
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
    auto currentNode = m_graph->getNodes()[costNode.id];
    if (currentNode.level >= m_labelsUntilLevel) {
      if (labelsToCollectMap.find(costNode.previousNode) ==
          labelsToCollectMap.end()) {
        labelsToCollectMap[costNode.previousNode] = std::vector<NodeId>();
        labelsToCollectMap[costNode.previousNode].emplace_back(costNode.id);
      } else
        labelsToCollectMap[costNode.previousNode].emplace_back(costNode.id);
      continue;
    }
    for (const auto &edge : m_graph->edgesFor(costNode.id, direction)) {
      if (m_graph->getLevel(edge.source) > m_graph->getLevel(edge.target))
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
  calcLabelTimingInfo.graphSearchTime = stopwatch.elapsedMicro();
  Stopwatch stopwatch1;
  for (auto [previousNodeId, labelsToCollect] : labelsToCollectMap) {
    for (auto id : labelsToCollect) {
      stopwatch1.reset();
      costNodeVec_t label;
      m_hubLabelStore->retrieve(id, direction, label);
      calcLabelTimingInfo.lookUpTime += stopwatch1.elapsedMicro();
      stopwatch1.reset();
      costNodeVec_t resultVec;
      resultVec.reserve(settledNodes.size() + label.size());
      Static::merge(settledNodes.begin(), settledNodes.end(),
                    label.begin(), label.end(), m_cost[id],
                    resultVec, PreviousReplacer(previousNodeId));
      settledNodes = std::move(resultVec);
      calcLabelTimingInfo.mergeTime += stopwatch1.elapsedMicro();
    }
  }
  return settledNodes;
}

template <typename Graph, typename CellIdStore>
std::vector<CHEdge> HybridPathFinder<Graph, CellIdStore>::
    getEdgeVectorFromNodeIdPath(const std::vector<NodeId> &path,
                                EdgeDirection direction) {
  std::vector<CHEdge> edgePath;
  for (int i = 0; i < path.size() - 1; ++i) {
    for (CHEdge edge : m_graph->edgesFor(path[i], direction)) {
      if (edge.target == path[i + 1])
        edgePath.emplace_back(edge);
    }
  }
  if(direction == EdgeDirection::BACKWARD) {
    for(auto& edge : edgePath) {
      Static::reverseEdge(edge);
    }
  }
  return edgePath;
}

template <typename Graph, typename CellIdStore>
void HybridPathFinder<Graph, CellIdStore>::addCellIds(
    const CHEdge &edge, std::vector<CellId_t> &result) {
  // edge always needs to be in forward order
  auto edgeId = m_graph->getEdgePosition(edge, EdgeDirection::FORWARD);
  if (!edgeId.has_value())
    throw std::runtime_error(
        "[Hublabels.addCellIds]could not find edgeId for edge(" +
        std::to_string(edge.source) + "," + std::to_string(edge.target) + ")");
  for (auto cellId : m_cellIdStore->getCellIds(edgeId.value())) {
    result.push_back(cellId);
  }
}

template <typename Graph, typename CellIdStore>
std::vector<NodeId>
HybridPathFinder<Graph, CellIdStore>::getShortestPathFromLabel(
    const costNodeVec_t &label, NodeId topNodeId, NodeId sourceId) {
  std::vector<NodeId> path;
  std::optional<CostNode> topNode = findElementInLabel(topNodeId, label);
  if (!topNode.has_value())
    throw std::runtime_error("could not find topNode in path");
  path.push_back(topNodeId);
  auto currentNode = topNode;
  while (currentNode.value().id != sourceId) {
    currentNode = findElementInLabel(currentNode.value().previousNode, label);
    if (!currentNode.has_value())
      throw std::runtime_error("could not find currentNode in path");
    path.push_back(currentNode.value().id);
  }
  return path;
}

template <typename Graph, typename CellIdStore>
std::optional<CostNode>
HybridPathFinder<Graph, CellIdStore>::findElementInLabel(
    NodeId nodeId, const costNodeVec_t &label) {
  // TODO: binary search here with std::lower_bound
  for (auto l : label) {
    if (l.id == nodeId)
      return l;
  }
  return std::nullopt;
}
template <typename Graph, typename CellIdStore>
size_t HybridPathFinder<Graph, CellIdStore>::graphNodeSize() {
  return m_graph->getNodes().size();
}
template <typename Graph, typename CellIdStore>
Level HybridPathFinder<Graph, CellIdStore>::labelsUntilLevel() {
  return m_labelsUntilLevel;
}
template <typename Graph, typename CellIdStore>
auto HybridPathFinder<Graph, CellIdStore>::getGraph() {
  return m_graph;
}
template <typename Graph, typename CellIdStore>
Level HybridPathFinder<Graph, CellIdStore>::getMaxLevel() {
  return m_hubLabelStore->maxLevel;
}
template <typename Graph, typename CellIdStore>
void HybridPathFinder<Graph, CellIdStore>::setLabelsUntilLevel(Level level) {
  m_labelsUntilLevel = level;
}
} // namespace pathFinder
#endif // MASTER_ARBEIT_HYBRIDPATHFINDER_H

#pragma clang diagnostic pop
