//
// Created by sokol on 16.06.20.
//
#include <path_finder/helper/Static.h>
#include <path_finder/routing/HubLabelCreator.h>

#include <utility>
namespace pathFinder {
HubLabelCreator::HubLabelCreator(
    CHGraph<std::vector> &graph, std::shared_ptr<HubLabelStore> hubLabelStore)
    : m_graph(graph), m_hubLabelStore(std::move(hubLabelStore)) {}


void HubLabelCreator::create(Level untilLevel) {
  m_labelsUntilLevel = untilLevel;
  m_graph.sortByLevel(m_sortedNodes);
  m_hubLabelStore->calculatedUntilLevel = untilLevel;

  // calculate node ranges with same level
  std::vector<std::pair<uint32_t, uint32_t>> sameLevelRanges;
  auto maxLevel = m_sortedNodes.begin()->level;
  m_hubLabelStore->maxLevel = maxLevel;
  auto currentLevel = m_sortedNodes.begin()->level;
  for (auto j = 0; j < m_sortedNodes.size(); ++j) {
    auto i = j;
    while (m_sortedNodes[j].level == currentLevel && j < m_sortedNodes.size()) {
      ++j;
    }
    sameLevelRanges.emplace_back(i, j);
    --j;
    currentLevel--;
  }

  constructAllLabels(sameLevelRanges, maxLevel, m_labelsUntilLevel);
}


void HubLabelCreator::constructAllLabels(
    const std::vector<std::pair<uint32_t, uint32_t>> &sameLevelRanges,
    Level maxLevel, Level labelsUntilLevel) {
  int currentLevel = maxLevel;
  for (const auto &sameLevelRange : sameLevelRanges) {
    std::cout << "constructing level: " << currentLevel << std::endl;
    processRangeParallel(sameLevelRange, EdgeDirection::FORWARD);
    processRangeParallel(sameLevelRange, EdgeDirection::BACKWARD);
    if (spaceMeasurer != nullptr)
      spaceMeasurer->setSpaceConsumption(currentLevel,
                                         m_hubLabelStore->getSpaceConsumption());
    if (--currentLevel < labelsUntilLevel)
      break;
  }
}

void HubLabelCreator::processRangeParallel(
    const std::pair<uint32_t, uint32_t> &range, EdgeDirection edgeDirection) {
//#pragma omp parallel for
  for (auto i = range.first; i < range.second; ++i) {
    auto id = m_sortedNodes[i].id;
    auto label = calcLabel(id, edgeDirection);
//#pragma omp critical
    m_hubLabelStore->store(label, id,
                          edgeDirection);
  }
}

std::vector<CostNode>
HubLabelCreator::calcLabel(NodeId nodeId,
                           EdgeDirection direction) {
  Level level = m_graph.getLevel(nodeId);
  costNodeVec_t label;
  costNodeVec_t newLabel;
  label.reserve(100);
  for (const auto &edge : m_graph.edgesFor(nodeId, direction)) {
    if (level < m_graph.getLevel(edge.target)) {
      auto targetLabel = m_hubLabelStore->retrieve(edge.target, direction);
      newLabel.clear();
      newLabel.reserve(targetLabel.size() + label.size());
      Static::merge(label.begin(), label.end(), targetLabel.begin(),
                    targetLabel.end(), edge.distance, newLabel,
                    PreviousReplacer(nodeId));
      label = std::move(newLabel);
    }
  }
  label.emplace_back(nodeId, 0, nodeId);
  int sizeBeforePrune = label.size();
  selfPrune(label, nodeId, direction);
  label.shrink_to_fit();
  Static::sortLabel(label);
  return label;
}

void HubLabelCreator::selfPrune(costNodeVec_t &label,
                                NodeId nodeId,
                                EdgeDirection direction) {
  bool forward = (direction == EdgeDirection::FORWARD);
  for (int i = (int)label.size() - 1; i >= 0; --i) {
    auto &[id, cost, previousNode] = label[i];
    const auto &otherLabels =
        m_hubLabelStore->retrieve(id, (EdgeDirection)!direction);
    std::optional<Distance> d = forward ? getShortestDistance(nodeId, id)
                                        : getShortestDistance(id, nodeId);
    if (d.has_value() && d.value() < cost) {
      label[i] = label[label.size() - 1];
      label.pop_back();
    }
  }
}

std::optional<Distance>
HubLabelCreator::getShortestDistance(NodeId source, NodeId target) {
  if (source >= m_graph.getNodes().size() ||
      target >= m_graph.getNodes().size())
    return std::nullopt;
  auto forwardLabels = m_hubLabelStore->retrieve(source, EdgeDirection::FORWARD);
  auto backwardLabels = m_hubLabelStore->retrieve(target, EdgeDirection::BACKWARD);
  NodeId topNode;
  auto d = Static::getShortestDistance(
      MyIterator(forwardLabels.begin(), forwardLabels.end()),
      MyIterator(backwardLabels.begin(), backwardLabels.end()),
      topNode);
  return d;
}
}
