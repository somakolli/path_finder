//
// Created by sokol on 17.06.20.
//
#include "path_finder/routing/HybridPathFinder.h"
namespace pathFinder {
RoutingResult
HybridPathFinder::getShortestPath(
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
        m_graph->getNode(newFinalForwardEdgePath[0].source).latLng);
  for (auto edge : newFinalForwardEdgePath) {
    newLatLngVector.push_back(m_graph->getNode(edge.target).latLng);
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


RoutingResult
HybridPathFinder::getShortestPath(
    LatLng source, LatLng target) {
  Stopwatch stopwatch;
  NodeId sourceId = m_graph->getNodeIdFor(source);
  NodeId targetId = m_graph->getNodeIdFor(target);
  auto nodeSearchTime = stopwatch.elapsedMicro();
  RoutingResult routingResult = getShortestPath(sourceId, targetId);
  routingResult.nodeSearchTime = nodeSearchTime;
  return routingResult;
}


std::vector<CostNode>
HybridPathFinder::calcLabelHybrid(
    NodeId source, EdgeDirection direction, CalcLabelTimingInfo& calcLabelTimingInfo) {
  Stopwatch stopwatch;
  if (m_graph->getLevel(source) >= m_labelsUntilLevel && m_hubLabelsCalculated) {
    CostNode* sourceLabel;
    size_t size;
    m_hubLabelStore->retrieve(source, direction, sourceLabel, size);
    costNodeVec_t vec;
    for (auto i = sourceLabel; i < sourceLabel + size; ++i)
      vec.push_back(*i);
    free(sourceLabel);
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
    auto currentNode = m_graph->getNode(costNode.id);
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
      CostNode* label;
      size_t labelSize;
      m_hubLabelStore->retrieve(id, direction, label, labelSize);
      calcLabelTimingInfo.lookUpTime += stopwatch1.elapsedMicro();
      stopwatch1.reset();
      costNodeVec_t resultVec;
      resultVec.reserve(settledNodes.size() + labelSize);
      Static::merge(settledNodes.begin(), settledNodes.end(),
                    label, label + labelSize, m_cost[id],
                    resultVec, PreviousReplacer(previousNodeId));
      free(label);
      settledNodes = std::move(resultVec);
      calcLabelTimingInfo.mergeTime += stopwatch1.elapsedMicro();
    }
  }
  return settledNodes;
}


std::vector<CHEdge> HybridPathFinder::
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


void HybridPathFinder::addCellIds(
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


std::vector<NodeId>
HybridPathFinder::getShortestPathFromLabel(
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


std::optional<CostNode>
HybridPathFinder::findElementInLabel(
    NodeId nodeId, const costNodeVec_t &label) {
  // TODO: binary search here with std::lower_bound
  for (auto l : label) {
    if (l.id == nodeId)
      return l;
  }
  return std::nullopt;
}

size_t HybridPathFinder::graphNodeSize() {
  return m_graph->getNumberOfNodes();
}

Level HybridPathFinder::labelsUntilLevel() {
  return m_labelsUntilLevel;
}

auto HybridPathFinder::getGraph() {
  return m_graph;
}

Level HybridPathFinder::getMaxLevel() {
  return m_hubLabelStore->maxLevel;
}

void HybridPathFinder::setLabelsUntilLevel(Level level) {
  m_labelsUntilLevel = level;
}
}