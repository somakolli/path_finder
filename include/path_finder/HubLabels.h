//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_HUBLABELS_H
#define ALG_ENG_PROJECT_HUBLABELS_H

#include "CHGraph.h"
#include "Graph.h"
#include "HubLabelStore.h"
#include "MmapVector.h"
#include "PathFinderBase.h"
#include "SpaceMeasurer.h"
#include "Static.h"
#include "Stopwatch.h"
#include "Timer.h"
#include "queue"
#include "thread"
#include <boost/container/small_vector.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <boost/format.hpp>

#define TIMER 1

namespace pathFinder {

typedef std::vector<CostNode> costNodeVec_t;
template <typename HubLabelStore = HubLabelStore<std::vector>,
          typename Graph = CHGraph<std::vector>>
class HubLabels : public PathFinderBase {
private:
  Level labelsUntilLevel = 0;
  HubLabelStore hubLabelStore;
  std::vector<CHNode> sortedNodes;
  std::vector<Distance> cost;
  std::vector<NodeId> visited;
  Graph &graph;

  Timer &_timer;


  // functions for preprocessing
  std::vector<CostNode> calcLabelPreprocessing(NodeId nodeId,
                                               EdgeDirection direction);
  std::optional<Distance> getShortestDistancePreprocessing(NodeId source,
                                                           NodeId target);
  void processRange(std::pair<uint32_t, uint32_t> range,
                    EdgeDirection direction);
  void processRangeParallel(std::pair<uint32_t, uint32_t> range,
                            EdgeDirection direction);
  void constructAllLabels(
      const std::vector<std::pair<uint32_t, uint32_t>> &sameLevelRanges,
      int maxLevel, int minLevel);
  void selfPrune(std::vector<CostNode> &labels, pathFinder::NodeId nodeId,
                 pathFinder::EdgeDirection direction);

  // functions for queries
  costNodeVec_t calcLabel(NodeId source, EdgeDirection direction);
  std::vector<NodeId> getShortestPath(const costNodeVec_t& label, NodeId topNode, NodeId sourceId);
  std::optional<CostNode> getElementFromLabel(NodeId id, const costNodeVec_t& label);
  std::vector<CHEdge> getEdgeVectorFromNodeIdPath(const std::vector<NodeId>& path, EdgeDirection direction);

public:
  SpaceMeasurer spaceMeasurer;
  HubLabels(Graph &graph, Level level, Timer &timer);
  HubLabels(Graph &graph, Level level, std::vector<CHNode> &sortedNodes,
            std::vector<Distance> &cost, Timer &timer);
  HubLabels(Graph &graph, Level level, HubLabelStore &hubLabelStore,
            Timer &timer);
  HubLabelStore &getHublabelStore();
  void setMinLevel(Level level);
  std::optional<Distance> getShortestDistance(NodeId source,
                                              NodeId target) override;
  void setTimer(Timer &timer);

  static std::optional<Distance>
  getShortestDistance(MyIterator<CostNode *> forwardLabels,
                      MyIterator<CostNode *> backwardLabels,
                      NodeId &nodeId);
  static std::optional<Distance>
  getShortestDistance(std::vector<CostNode>& forwardLabels,
                    std::vector<CostNode>& backwardLabels,
                    NodeId &nodeId);
  static void sortLabel(costNodeVec_t &label);
  std::vector<LatLng> getShortestPath(NodeId source, NodeId target) override;
  void writeToFile(boost::filesystem::path filePath);
  void setLabelsUntilLevel(Level level);

};

template <typename HubLabelStore, typename Graph>
pathFinder::HubLabels<HubLabelStore, Graph>::HubLabels(Graph &graph,
                                                       Level level,
                                                       Timer &timer)
    : graph(graph), labelsUntilLevel(level),
      hubLabelStore(graph.getNodes().size()), _timer(timer) {
  cost.reserve(graph.getNodes().size());
  while (cost.size() < graph.getNodes().size())
    cost.push_back(MAX_DISTANCE);

  graph.sortByLevel(sortedNodes);

  // calculate node ranges with same level
  std::vector<std::pair<uint32_t, uint32_t>> sameLevelRanges;
  auto maxLevel = sortedNodes.begin()->level;
  auto currentLevel = sortedNodes.begin()->level;
  for (auto j = 0; j < sortedNodes.size(); ++j) {
    auto i = j;
    while (sortedNodes[j].level == currentLevel && j < sortedNodes.size()) {
      ++j;
    }
    sameLevelRanges.emplace_back(i, j + 1);
    currentLevel--;
  }
  auto start = std::chrono::steady_clock::now();
  std::cout << "constructing labels..." << std::endl;
  constructAllLabels(sameLevelRanges, maxLevel, labelsUntilLevel);

  auto end = std::chrono::steady_clock::now();
  std::cout
      << "Constructed Labels in  "
      << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
      << " s" << std::endl;
}

template <typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::constructAllLabels(
    const std::vector<std::pair<uint32_t, uint32_t>> &sameLevelRanges,
    int maxLevel, int minLevel) {
  int currentLevel = maxLevel;
  for (const auto &sameLevelRange : sameLevelRanges) {
    std::cout << "constructing level: " << currentLevel << std::endl;
    processRangeParallel(sameLevelRange, EdgeDirection::FORWARD);
    processRangeParallel(sameLevelRange, EdgeDirection::BACKWARD);
    spaceMeasurer.setSpaceConsumption(currentLevel,
                                      hubLabelStore.getSpaceConsumption());
    if (--currentLevel < minLevel)
      break;
  }
}

template <typename HubLabelStore, typename Graph>
std::optional<pathFinder::Distance>
pathFinder::HubLabels<HubLabelStore, Graph>::getShortestDistance(
    pathFinder::NodeId source, pathFinder::NodeId target) {
#ifdef TIMER
  Stopwatch sw;
#endif
  if (source >= graph.getNodes().size() || target >= graph.getNodes().size())
    return std::nullopt;
  auto forwardLabels = calcLabel(source, EdgeDirection::FORWARD);
  auto backwardLabels = calcLabel(target, EdgeDirection::BACKWARD);
  NodeId topNode;
  auto d = getShortestDistance(
      MyIterator(forwardLabels.begin().base(), forwardLabels.end().base()),
      MyIterator(backwardLabels.begin().base(), backwardLabels.end().base()),
      topNode);
  return d;
}

template <typename HubLabelStore, typename Graph>
std::optional<pathFinder::Distance>
pathFinder::HubLabels<HubLabelStore, Graph>::getShortestDistancePreprocessing(
    pathFinder::NodeId source, pathFinder::NodeId target) {
  if (source >= graph.getNodes().size() || target >= graph.getNodes().size())
    return std::nullopt;
  auto forwardLabels = hubLabelStore.retrieve(source, EdgeDirection::FORWARD);
  auto backwardLabels = hubLabelStore.retrieve(target, EdgeDirection::BACKWARD);
  NodeId topNode;
  return getShortestDistance(forwardLabels, backwardLabels, topNode);
}

template <typename HubLabelStore, typename Graph>
std::optional<pathFinder::Distance>
pathFinder::HubLabels<HubLabelStore, Graph>::getShortestDistance(
    MyIterator<CostNode *> forwardLabels, MyIterator<CostNode *> backwardLabels,
    NodeId &topNode) {
  Distance shortestDistance = MAX_DISTANCE;
  topNode = 0;
  auto i = forwardLabels.begin();
  auto j = backwardLabels.begin();
  while (i != forwardLabels.end() && j != backwardLabels.end()) {
    auto &forwardCostNode = *i;
    auto &backwardCostNode = *j;
    if (forwardCostNode.id == backwardCostNode.id) {
      if (forwardCostNode.cost + backwardCostNode.cost < shortestDistance){
        shortestDistance = forwardCostNode.cost + backwardCostNode.cost;
        topNode = forwardCostNode.id;
      }
      ++j;
      ++i;
    } else if (forwardCostNode.id < backwardCostNode.id)
      ++i;
    else
      ++j;
  }
  return shortestDistance;
}

template <typename HubLabelStore, typename Graph>
std::vector<pathFinder::CostNode>
pathFinder::HubLabels<HubLabelStore, Graph>::calcLabelPreprocessing(
    pathFinder::NodeId nodeId, pathFinder::EdgeDirection direction) {
  Level level = graph.getLevel(nodeId);
  costNodeVec_t label;
  costNodeVec_t newLabel;
  label.reserve(100);
  for (const auto &edge : graph.edgesFor(nodeId, direction)) {
    if (level < graph.getLevel(edge.target)) {
      auto targetLabel = hubLabelStore.retrieve(edge.target, direction);
      newLabel.clear();
      newLabel.reserve(targetLabel.size() + label.size());
      Static::merge(label.begin(), label.end(), targetLabel.begin(),
                    targetLabel.end(), edge.distance, newLabel, PreviousReplacer(nodeId));
      label = std::move(newLabel);
    }
  }
  label.emplace_back(nodeId, 0, nodeId);
  int sizeBeforePrune = label.size();
  selfPrune(label, nodeId, direction);
  label.shrink_to_fit();
  sortLabel(label);
  return label;
}

template <typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::sortLabel(
    costNodeVec_t &label) {
  std::sort(label.begin(), label.end(),
            [](const CostNode &node1, const CostNode &node2) {
              return node1.id == node2.id ? node1.cost < node2.cost
                                          : node1.id < node2.id;
            });
}

template <typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::processRange(
    std::pair<uint32_t, uint32_t> range, EdgeDirection direction) {
  for (auto i = range.first; i < range.second; ++i) {
    auto label = calcLabel(sortedNodes[i].id, direction);
    hubLabelStore.store(label, sortedNodes[i].id, direction);
  }
}

template <typename HubLabelStore, typename Graph>
void HubLabels<HubLabelStore, Graph>::processRangeParallel(
    std::pair<uint32_t, uint32_t> range, EdgeDirection direction) {
  std::vector<costNodeVec_t> labelsForRange;
  labelsForRange.reserve(range.second - range.first);
  for (auto i = range.first; i < range.second; ++i)
    labelsForRange.emplace_back(costNodeVec_t());
#pragma omp parallel for
  for (auto i = range.first; i < range.second; ++i) {
    labelsForRange[i - range.first] =
        calcLabelPreprocessing(sortedNodes[i].id, direction);
  }
  int i = 0;
  for (auto &label : labelsForRange) {
    hubLabelStore.store(label, sortedNodes[range.first + i++].id, direction);
    label.clear();
    label.shrink_to_fit();
  }
}

template <typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::selfPrune(
    std::vector<CostNode> &labels, pathFinder::NodeId nodeId,
    pathFinder::EdgeDirection direction) {
  bool forward = (direction == EdgeDirection::FORWARD);
  for (int i = labels.size() - 1; i >= 0; --i) {
    auto &[id, cost, previousNode] = labels[i];
    const auto &otherLabels =
        hubLabelStore.retrieve(id, (EdgeDirection)!direction);
    std::optional<Distance> d = forward ? getShortestDistance(nodeId, id)
                     : getShortestDistance(id, nodeId);
    if (d.value() < cost) {
      labels[i] = labels[labels.size() - 1];
      labels.pop_back();
    }
  }
}

template <typename HubLabelStore, typename Graph>
std::vector<pathFinder::LatLng>
pathFinder::HubLabels<HubLabelStore, Graph>::getShortestPath(
    pathFinder::NodeId source, pathFinder::NodeId target) {
  auto forwardLabel = calcLabel(source, EdgeDirection::FORWARD);
  auto backwardLabel = calcLabel(target, EdgeDirection::BACKWARD);
  NodeId topNode;
  std::optional<Distance> shortestDistance = getShortestDistance(forwardLabel, backwardLabel, topNode);

  // the forward path is in reverse the backward path is correct
  auto reverseForwardPath = getShortestPath(forwardLabel, source, topNode);
  auto backwardPath = getShortestPath(backwardLabel, target, topNode);

  // reverse the forward path
  std::vector<NodeId> forwardPath;
  for(int i = reverseForwardPath.size(); i >= 0; --i) {
    forwardPath.push_back(reverseForwardPath[i]);
  }

  // get edges to unpack them

  std::vector<CHEdge> forwardEdges = getEdgeVectorFromNodeIdPath(forwardPath, EdgeDirection::FORWARD);
  std::vector<CHEdge> backwardEdges = getEdgeVectorFromNodeIdPath(backwardPath, EdgeDirection::BACKWARD);

  // unpack edges
  std::vector<NodeId> finalPath;
  for(auto edge : forwardEdges)
    for(auto id : graph.getPathFromShortcut(edge, 0))
      finalPath.emplace_back(id);

  // get lat longs
  std::vector<LatLng> latLngVector;
  for(auto id : finalPath) {
    latLngVector.push_back(graph.getNodes()[id].latLng);
  }

  return latLngVector;
}

template <typename HubLabelStore, typename Graph>
pathFinder::costNodeVec_t
pathFinder::HubLabels<HubLabelStore, Graph>::calcLabel(
    NodeId source, EdgeDirection direction) {
  const auto &sourceLabel = hubLabelStore.retrieve(source, direction);
  if (graph.getLevel(source) >= labelsUntilLevel) {
    costNodeVec_t vec;
    for (const auto entry : sourceLabel)
      vec.push_back(entry);
    return vec;
  }
  std::vector<CostNode> settledNodes;
  std::map<NodeId, std::vector<NodeId>> labelsToCollectMap;
  for (auto nodeId : visited)
    cost[nodeId] = MAX_DISTANCE;
  visited.clear();
  std::priority_queue<CostNode> q;
  q.emplace(source, 0, source);
  cost[source] = 0;
  visited.emplace_back(source);
  // ch dijkstra
  while (!q.empty()) {
    auto costNode = q.top();
    q.pop();
    if (costNode.cost > cost[costNode.id])
      continue;
    settledNodes.emplace_back(costNode.id, costNode.cost,
                              costNode.previousNode);
    auto currentNode = graph.getNodes()[costNode.id];
    if (currentNode.level >= labelsUntilLevel) {
      if(labelsToCollectMap.find(costNode.previousNode) == labelsToCollectMap.end()) {
        labelsToCollectMap[costNode.previousNode] = std::vector<NodeId>();
        labelsToCollectMap[costNode.previousNode].emplace_back(costNode.id);
      }
      else
        labelsToCollectMap[costNode.previousNode].emplace_back(costNode.id);
      continue;
    }
    for (const auto &edge : graph.edgesFor(costNode.id, direction)) {
      if (graph.getLevel(edge.source) > graph.getLevel(edge.target))
        continue;
      auto addedCost = costNode.cost + edge.distance;
      if (addedCost < cost[edge.target]) {
        visited.emplace_back(edge.target);
        cost[edge.target] = addedCost;
        q.emplace(edge.target, addedCost, edge.source);
      }
    }
  }
  sortLabel(settledNodes);
  for (auto [previousNodeId,labelsToCollect]  : labelsToCollectMap) {
    for (auto id : labelsToCollect) {
      costNodeVec_t targetLabel;
      for (const auto &entry : hubLabelStore.retrieve(id, direction))
        targetLabel.push_back(entry);
      costNodeVec_t resultVec;
      resultVec.reserve(settledNodes.size() + targetLabel.size());
      Static::merge(settledNodes.begin(), settledNodes.end(),
                    targetLabel.begin(), targetLabel.end(), cost[id], resultVec,
                    PreviousReplacer(previousNodeId));
      settledNodes = std::move(resultVec);
    }
  }
  return settledNodes;
}

template <typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::setMinLevel(
    pathFinder::Level level) {
  this->labelsUntilLevel = level;
}
template <typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::writeToFile(
    boost::filesystem::path filePath) {
  // TODO implement maybe
}

template <typename HubLabelStore, typename Graph>
pathFinder::HubLabels<HubLabelStore, Graph>::HubLabels(
    Graph &graph, pathFinder::Level level, HubLabelStore &hubLabelStore,
    Timer &timer)
    : graph(graph), labelsUntilLevel(level), hubLabelStore(hubLabelStore),
      _timer(timer) {
  graph.sortByLevel(sortedNodes);
  cost.reserve(graph.getNodes().size());
  while (cost.size() < graph.getNodes().size())
    cost.push_back(MAX_DISTANCE);
}

template <typename HubLabelStore, typename Graph>
HubLabelStore &HubLabels<HubLabelStore, Graph>::getHublabelStore() {
  return hubLabelStore;
}

template <typename HubLabelStore, typename Graph>
HubLabels<HubLabelStore, Graph>::HubLabels(Graph &graph, Level level,
                                           std::vector<CHNode> &sortedNodes,
                                           std::vector<Distance> &cost,
                                           Timer &timer)
    : graph(graph), labelsUntilLevel(level),
      hubLabelStore(graph.getNodes().size()), _timer(timer) {
  this->graph = graph;
  this->labelsUntilLevel = level;
  this->sortedNodes = sortedNodes;
  this->cost = cost;

  // calculate node ranges with same level
  std::vector<std::pair<uint32_t, uint32_t>> sameLevelRanges;
  auto maxLevel = sortedNodes.begin()->level;
  auto currentLevel = sortedNodes.begin()->level;
  for (int j = 0; j < sortedNodes.size(); ++j) {
    auto i = j;
    while (sortedNodes[j].level == currentLevel && j < sortedNodes.size()) {
      ++j;
    }
    sameLevelRanges.emplace_back(i, j + 1);
    currentLevel--;
  }
  auto start = std::chrono::steady_clock::now();
  std::cout << "constructing labels..." << std::endl;
  constructAllLabels(sameLevelRanges, maxLevel, labelsUntilLevel);

  // construct forward and backward labels
  auto end = std::chrono::steady_clock::now();
  std::cout
      << "Constructed Labels in  "
      << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
      << " s" << std::endl;
}

template <typename HubLabelStore, typename Graph>
void HubLabels<HubLabelStore, Graph>::setTimer(Timer &timer) {
  this->_timer = timer;
}

template <typename HubLabelStore, typename Graph>
void HubLabels<HubLabelStore, Graph>::setLabelsUntilLevel(Level level) {
  labelsUntilLevel = level;
}
template <typename HubLabelStore, typename Graph>
std::vector<NodeId>
HubLabels<HubLabelStore, Graph>::getShortestPath(const costNodeVec_t& label,
                                                 NodeId sourceId,
                                                 NodeId topNodeId) {
  std::vector<NodeId> path;
  std::optional<CostNode> topNode = getElementFromLabel(topNodeId, label);
  if(!topNode.has_value())
    throw std::runtime_error("could not find topNode in path");
  path.push_back(topNodeId);
  auto currentNode = topNode;
  while(currentNode.value().id != sourceId) {
    currentNode = getElementFromLabel(currentNode.value().previousNode, label);
    if(!currentNode.has_value())
      throw std::runtime_error("could not find currentNode in path");
    path.push_back(currentNode.value().id);
  }
  return path;
}
template <typename HubLabelStore, typename Graph>
std::optional<CostNode> HubLabels<HubLabelStore, Graph>::getElementFromLabel(
    NodeId id, const costNodeVec_t &label) {
  // could do binary search here
  for(auto l : label) {
    if(l.id == id)
      return l;
  }
  return std::nullopt;
}
template <typename HubLabelStore, typename Graph>
std::vector<CHEdge>
HubLabels<HubLabelStore, Graph>::getEdgeVectorFromNodeIdPath(
    const std::vector<NodeId>& path,
    EdgeDirection direction) {
  std::vector<CHEdge> edgePath;
  for(int i = 0; i < path.size() - 1; ++i) {
    for(CHEdge edge : graph.edgesFor(path[i], direction)) {
      if(edge.target == path[i+1])
        edgePath.emplace_back(edge);
    }
  }
  return edgePath;
}
template <typename HubLabelStore, typename Graph>
std::optional<Distance> HubLabels<HubLabelStore, Graph>::getShortestDistance(
    std::vector<CostNode>& forwardLabels, std::vector<CostNode>& backwardLabels,
    NodeId &nodeId) {
  return getShortestDistance(
      MyIterator(forwardLabels.begin().base(), forwardLabels.end().base()),
      MyIterator(backwardLabels.begin().base(), backwardLabels.end().base()),
      nodeId);
}
} // namespace pathFinder
#endif // ALG_ENG_PROJECT_HUBLABELS_H
