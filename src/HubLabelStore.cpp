//
// Created by sokol on 23.01.20.
//

#include "path_finder/storage/HubLabelStore.h"
#include <cstring>
#include <execution>
#include <memory>
#include <path_finder/helper/Static.h>
#include <sstream>
#include <utility>

namespace pathFinder {
HubLabelStore::HubLabelStore(size_t numberOfLabels) {
  m_forwardOffset = new OffsetElement[numberOfLabels];
  m_backwardOffset = new OffsetElement[numberOfLabels];
  m_backwardLabels = nullptr;
  m_forwardLabels = nullptr;
  this->m_numberOfLabels = numberOfLabels;
}

void HubLabelStore::store(const std::vector<CostNode> &label, NodeId id, EdgeDirection direction) {
  auto &storePointer = direction ? m_forwardLabels : m_backwardLabels;
  auto &offsetPointer = direction ? m_forwardOffset : m_backwardOffset;
  auto &size = direction ? m_forwardLabelSize : m_backwardLabelSize;
  offsetPointer[id] = OffsetElement{size, (uint32_t)label.size()};
  {
	auto * newData = new CostNode[size + label.size()];
	std::copy(storePointer, storePointer+size, newData);
	std::copy(label.begin(), label.end(), storePointer+size);
	std::swap(newData, storePointer);
	delete[] newData;
  }
  size += label.size();
}

void HubLabelStore::store(std::vector<HubLabelStore::IdLabelPair> &idLabels, EdgeDirection direction) {
  size_t allocationSize = std::accumulate(
      idLabels.begin(), idLabels.end(), (size_t)0,
      [](size_t init, const HubLabelStore::IdLabelPair &idLabelPair) { return init + idLabelPair.second.size(); });

  auto &storePointer = direction ? m_forwardLabels : m_backwardLabels;
  auto &offsetPointer = direction ? m_forwardOffset : m_backwardOffset;
  auto &size = direction ? m_forwardLabelSize : m_backwardLabelSize;
  {
	CostNode * newData = new CostNode[size + allocationSize];
	std::copy(storePointer, storePointer+size, newData);
	std::swap(newData, storePointer);
	delete[] newData;
  }
  std::cout << "writing labels: 0/" << idLabels.size() << '\r';
  int i = 0;
  for (auto &&[id, label] : idLabels) {
    offsetPointer[id] = OffsetElement{size, (uint32_t)label.size()};
    std::memcpy(storePointer + size, label.data(), sizeof(CostNode) * label.size());
    size += label.size();
    label.clear();
    char newLineChar = (i == idLabels.size() - 1) ? '\n' : '\r';
    std::cout << "writing labels: " << ++i << '/' << idLabels.size() << newLineChar;
  }
}

MyIterator<const CostNode *> HubLabelStore::retrieveIt(NodeId id, EdgeDirection direction) const {
  if (id > m_numberOfLabels - 1)
    throw std::runtime_error("[HublabelStore] node id does not exist.");
  auto offsetElement = direction ? m_forwardOffset[id] : m_backwardOffset[id];
  auto labelEnd = offsetElement.position + offsetElement.size;
  const auto &storePointer = direction ? m_forwardLabels : m_backwardLabels;
  return MyIterator<const CostNode *>(storePointer + offsetElement.position, storePointer + labelEnd);
}

std::vector<CostNode> HubLabelStore::retrieve(NodeId id, EdgeDirection direction) const {
  if (id > m_numberOfLabels - 1)
    throw std::runtime_error("[HublabelStore] node id does not exist.");
  bool forward = (EdgeDirection::FORWARD == direction);
  auto offsetElement = forward ? m_forwardOffset[id] : m_backwardOffset[id];
  if (offsetElement.size == 0) {
    return std::vector<CostNode>();
  }
  std::vector<CostNode> storeVec;
  storeVec.reserve(offsetElement.size);
  auto labelEnd = offsetElement.position + offsetElement.size;
  volatile const auto &storePointer = forward ? m_forwardLabels : m_backwardLabels;
  for (auto i = offsetElement.position; i < labelEnd; ++i) {
    storeVec.push_back(storePointer[i]);
  }
  return storeVec;
}
void HubLabelStore::retrieve(NodeId id, EdgeDirection direction, CostNode *&storeVec, size_t &size) const {
  if (id > m_numberOfLabels - 1)
    throw std::runtime_error("[HublabelStore] node id does not exist.");
  bool forward = (EdgeDirection::FORWARD == direction);
  auto offsetElement = forward ? m_forwardOffset[id] : m_backwardOffset[id];
  if (offsetElement.size == 0) {
    storeVec = nullptr;
    size = 0;
    return;
  }
  auto labelEnd = offsetElement.position + offsetElement.size;
  storeVec = new CostNode[offsetElement.size];
  size = offsetElement.size;
  const auto storePointer = forward ? m_forwardLabels : m_backwardLabels;
  for (auto i = offsetElement.position, j = (size_t)0; i < labelEnd; ++i, ++j) {
    storeVec[j] = storePointer[i];
  }
}

size_t HubLabelStore::getNumberOfLabels() const { return m_numberOfLabels; }

HubLabelStore::HubLabelStore(HubLabelStoreInfo hubLabelStoreInfo) {
  this->m_forwardLabels = hubLabelStoreInfo.forwardLabels;
  this->m_backwardLabels = hubLabelStoreInfo.backwardLabels;

  this->m_forwardOffset = hubLabelStoreInfo.forwardOffset;
  this->m_backwardOffset = hubLabelStoreInfo.backwardOffset;

  this->m_numberOfLabels = hubLabelStoreInfo.numberOfLabels;
  this->m_forwardLabelSize = hubLabelStoreInfo.forwardLabelSize;
  this->m_backwardLabelSize = hubLabelStoreInfo.backwardLabelSize;

  this->m_forwardLabelsMMap = hubLabelStoreInfo.forwardLabelsMMap;
  this->m_backwardLabelsMMap = hubLabelStoreInfo.backwardLabelsMMap;
  this->m_forwardOffsetMMap = hubLabelStoreInfo.forwardOffsetMMap;
  this->m_backwardOffsetMMap = hubLabelStoreInfo.backwardOffsetMMap;

  this->calculatedUntilLevel = hubLabelStoreInfo.calculatedUntilLevel;
}

HubLabelStore::~HubLabelStore() {
  Static::conditionalFree(m_forwardLabels, m_forwardLabelsMMap, m_forwardLabelSize * sizeof(CostNode));
  Static::conditionalFree(m_backwardLabels, m_backwardLabelsMMap, m_backwardLabelSize * sizeof(CostNode));
  Static::conditionalFree(m_forwardOffset, m_forwardOffsetMMap, m_numberOfLabels * sizeof(OffsetElement));
  Static::conditionalFree(m_backwardOffset, m_backwardOffsetMMap, m_numberOfLabels * sizeof(OffsetElement));
}
size_t HubLabelStore::getForwardLabelsSize() const { return m_forwardLabelSize; }
size_t HubLabelStore::getBackwardLabelsSize() const { return m_backwardLabelSize; }
size_t HubLabelStore::getForwardOffsetSize() const { return m_numberOfLabels; }
size_t HubLabelStore::getBackwardOffsetSize() const { return m_numberOfLabels; }
[[maybe_unused]] std::string HubLabelStore::printStore() const {
  std::stringstream ss;
  for (NodeId i = 0; i < m_numberOfLabels; ++i) {
    std::vector<CostNode> forwardVec = retrieve(i, EdgeDirection::FORWARD);
    std::vector<CostNode> backwardVec = retrieve(i, EdgeDirection::BACKWARD);

    ss << "label for id: " << i << '\n';
    ss << "forward:" << '\n';
    for (const auto &[id, cost, previousNode] : forwardVec) {
      ss << id << ' ' << cost << ' ' << previousNode << '\n';
    }
    ss << "backward:" << '\n';
    for (const auto &[id, cost, previousNode] : backwardVec) {
      ss << id << ' ' << cost << ' ' << previousNode << '\n';
    }
  }

  return ss.str();
}
std::shared_ptr<HubLabelStore> HubLabelStore::makeShared(size_t numberOfLabels) {
  return std::make_shared<HubLabelStore>(numberOfLabels);
}
auto HubLabelStore::getSpaceConsumption() const -> size_t {
    return (m_forwardLabelSize + m_backwardLabelSize) * sizeof(CostNode)
         + (m_numberOfLabels + m_numberOfLabels) * sizeof (OffsetElement);

};

} // namespace pathFinder
