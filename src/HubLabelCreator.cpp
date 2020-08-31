//
// Created by sokol on 16.06.20.
//
#include <path_finder/helper/Static.h>
#include <path_finder/routing/HubLabelCreator.h>

#include <algorithm>
#include <execution>
#include <mutex>
#include <utility>
namespace pathFinder {
HubLabelCreator::HubLabelCreator(std::shared_ptr<CHGraph> graph, std::shared_ptr<HubLabelStore> hubLabelStore)
    : m_graph(graph), m_hubLabelStore(std::move(hubLabelStore)) {}

void HubLabelCreator::create(Level untilLevel) {
  m_labelsUntilLevel = untilLevel;
  m_graph->sortByLevel(m_sortedNodes);
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

void HubLabelCreator::constructAllLabels(const std::vector<std::pair<uint32_t, uint32_t>> &sameLevelRanges,
                                         Level maxLevel, Level labelsUntilLevel) {
  int currentLevel = maxLevel;
  for (const auto &sameLevelRange : sameLevelRanges) {
    std::cout << "---------------------------------\n";
    std::cout << "constructing level: " << currentLevel << std::endl;
    processRangeParallel(sameLevelRange, EdgeDirection::FORWARD);
    processRangeParallel(sameLevelRange, EdgeDirection::BACKWARD);
    if (spaceMeasurer != nullptr)
      spaceMeasurer->setSpaceConsumption(currentLevel, m_hubLabelStore->getSpaceConsumption());
    if (--currentLevel < labelsUntilLevel)
      break;
  }
}

void HubLabelCreator::processRangeParallel(const std::pair<uint32_t, uint32_t> &range, EdgeDirection edgeDirection) {
  auto begin = &m_sortedNodes[range.first];
  auto end = &m_sortedNodes[range.second];
  std::mutex m;
  std::vector<std::pair<NodeId, costNodeVec_t>> labels;
  size_t rangeSize = range.second - range.first;
  labels.resize(rangeSize);
  std::cout << "calculating " << rangeSize << " labels in parallel...\n";
  std::transform(std::execution::par_unseq, begin, end, labels.begin(), [&](auto &&item) {
    auto id = item.id;
    auto label = calcLabel(id, edgeDirection);
    return std::make_pair(id, label);
  });
  m_hubLabelStore->store(labels, edgeDirection);
}

std::vector<CostNode> HubLabelCreator::calcLabel(NodeId nodeId, EdgeDirection direction) const {
  Level level = m_graph->getLevel(nodeId);
  std::vector<CostNode> label;
  label.reserve(1000);
  for (const auto &edge : m_graph->edgesFor(nodeId, direction)) {
    if (level < m_graph->getLevel(edge.target)) {
      auto targetLabel = m_hubLabelStore->retrieveIt(edge.target, direction);
      std::byte stackBuffer[sizeof(CostNode) * (label.size() + targetLabel.size())];
      std::pmr::monotonic_buffer_resource rsrc(stackBuffer, sizeof(stackBuffer));
      std::pmr::vector<CostNode> result{&rsrc};
      result.reserve(label.size() + targetLabel.size());
      Static::merge(label.begin(), label.end(), targetLabel.begin(), targetLabel.end(),
                                             edge.distance, PreviousReplacer(nodeId), result);
      label.clear();
      for(const auto& costNode: result)
        label.emplace_back(costNode);
    }
  }
  label.emplace_back(nodeId, 0, nodeId);
  Static::sortLabel(label);
  selfPrune(label, nodeId, direction);
  label.shrink_to_fit();
  Static::sortLabel(label);
  return label;
}

void HubLabelCreator::selfPrune(std::vector<CostNode> &label, NodeId nodeId, EdgeDirection direction) const {
  auto other = (pathFinder::EdgeDirection)!direction;
  for (int i = (int)label.size() - 1; i >= 0; --i) {
    auto &[id, cost, previousNode] = label[i];
    auto otherLabels = m_hubLabelStore->retrieveIt(id, other);
    NodeId topNode;
    auto d = Static::getShortestDistance(MyIterator(label.begin().base(), label.end().base()), otherLabels, topNode);
    if (d.has_value() && d.value() < cost) {
      label[i] = label[label.size() - 1];
      label.pop_back();
    }
  }
}

std::optional<Distance> HubLabelCreator::getShortestDistance(NodeId source, NodeId target) {
  if (!m_graph->isValidNodeId(source) || !m_graph->isValidNodeId(target))
    return std::nullopt;
  CostNode *forwardLabels;
  size_t forwardSize;
  CostNode *backwardLabels;
  size_t backwardSize;
  m_hubLabelStore->retrieve(source, EdgeDirection::FORWARD, forwardLabels, forwardSize);
  m_hubLabelStore->retrieve(target, EdgeDirection::BACKWARD, backwardLabels, backwardSize);
  NodeId topNode;
  auto d = Static::getShortestDistance(MyIterator(forwardLabels, forwardLabels + forwardSize),
                                       MyIterator(backwardLabels, backwardLabels + backwardSize), topNode);
  free(forwardLabels);
  free(backwardLabels);
  return d;
}
} // namespace pathFinder
