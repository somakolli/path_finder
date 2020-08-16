//
// Created by sokol on 23.01.20.
//

#include "path_finder/storage/HubLabelStore.h"
#include <cstring>
#include <memory>
#include <path_finder/helper/Static.h>
#include <sstream>
#include <utility>

namespace pathFinder {
HubLabelStore::HubLabelStore(size_t numberOfLabels) {
  m_forwardOffset = (OffsetElement*) std::calloc( numberOfLabels , sizeof(OffsetElement));
  m_backwardOffset = (OffsetElement*) std::calloc( numberOfLabels , sizeof(OffsetElement));
  m_backwardLabels = nullptr;
  m_forwardLabels = nullptr;
  this->m_numberOfLabels = numberOfLabels;
}

void HubLabelStore::store(
    const std::vector<CostNode> &label, NodeId id, EdgeDirection direction) {
  if(true) {
    if (direction == EdgeDirection::FORWARD) {
      m_forwardOffset[id] =
          OffsetElement{m_forwardLabelSize, (uint32_t)label.size()};
      m_forwardLabels = (CostNode*) std::realloc(m_forwardLabels, sizeof(CostNode) * (m_forwardLabelSize + label.size()));
      std::memcpy(m_forwardLabels + m_forwardLabelSize, label.data(), sizeof(CostNode) * label.size());
      m_forwardLabelSize += label.size();
    } else {
      m_backwardOffset[id] =
          OffsetElement{m_backwardLabelSize, (uint32_t)label.size()};
      m_backwardLabels = (CostNode*) std::realloc(m_backwardLabels, sizeof(CostNode) * (m_backwardLabelSize + label.size()));
      std::memcpy(m_backwardLabels + m_backwardLabelSize, label.data(), sizeof(CostNode) * label.size());
      m_backwardLabelSize += label.size();
    }
  }
}

void HubLabelStore::retrieve(NodeId id, EdgeDirection direction, std::vector<CostNode>& storeVec) {
  if(id > m_numberOfLabels - 1)
    throw std::runtime_error("[HublabelStore] node id does not exist.");
  if (direction == EdgeDirection::FORWARD) {
    auto offsetElement = m_forwardOffset[id];
    storeVec.reserve(offsetElement.size);
    auto labelEnd = offsetElement.position + offsetElement.size;
    for(auto i = offsetElement.position; i < labelEnd; ++i) {
      CostNode costNode = m_forwardLabels[i];
      storeVec.emplace_back(costNode);
    }
  } else if(direction == EdgeDirection::BACKWARD) {
    auto offsetElement = m_backwardOffset[id];
    storeVec.reserve(offsetElement.size);
    auto labelEnd = offsetElement.position + offsetElement.size;
    for(auto i = offsetElement.position; i < labelEnd; ++i) {
      CostNode costNode = m_backwardLabels[i];
      storeVec.emplace_back(costNode);
    }
  }
}
void HubLabelStore::retrieve(NodeId id, EdgeDirection direction, CostNode *&storeVec, size_t& size) {
  if(id > m_numberOfLabels - 1)
    throw std::runtime_error("[HublabelStore] node id does not exist.");
  if (direction == EdgeDirection::FORWARD) {
    auto offsetElement = m_forwardOffset[id];
    if(offsetElement.size == 0) {
      storeVec = nullptr;
      size = 0;
      return;
    }
    auto labelEnd = offsetElement.position + offsetElement.size;
    storeVec = (CostNode*) malloc(sizeof(CostNode) * offsetElement.size);
    size = offsetElement.size;
    int j = 0;
    for(auto i = offsetElement.position; i < labelEnd; ++i) {
//#pragma omp critical
      std::memcpy(storeVec + j++, m_forwardLabels + i, sizeof(CostNode));
    }
  } else if(direction == EdgeDirection::BACKWARD) {
    auto offsetElement = m_backwardOffset[id];
    if(offsetElement.size == 0) {
      storeVec = nullptr;
      size = 0;
      return;
    }
    auto labelEnd = offsetElement.position + offsetElement.size;
    storeVec = (CostNode*) malloc(sizeof(CostNode) * offsetElement.size);
    size = offsetElement.size;
    int j = 0;
    for(auto i = offsetElement.position; i < labelEnd; ++i) {
//#pragma omp critical
      std::memcpy(storeVec + j++, m_backwardLabels + i, sizeof(CostNode));
    }
  }
}

size_t HubLabelStore::getNumberOfLabels() const  {
  return m_numberOfLabels;
}


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
std::string HubLabelStore::printStore() {
  std::stringstream ss;
  for(NodeId i = 0; i < m_numberOfLabels; ++i) {
    std::vector<CostNode> forwardVec;
    std::vector<CostNode> backwardVec;
    retrieve(i, EdgeDirection::FORWARD, forwardVec);
    retrieve(i, EdgeDirection::BACKWARD, backwardVec);
    ss << "label for id: " << i << '\n';
    ss << "forward:" << '\n';
    for(const auto& [id, cost , previousNode]: forwardVec) {
      ss << id << ' ' << cost << ' ' << previousNode << '\n';
    }
    ss << "backward:" << '\n';
    for(const auto& [id, cost , previousNode]: backwardVec) {
      ss << id << ' ' << cost << ' ' << previousNode << '\n';
    }
  }

  return ss.str();
}
std::shared_ptr<HubLabelStore> HubLabelStore::makeShared(size_t numberOfLabels) {
  return std::make_shared<HubLabelStore>(numberOfLabels);
}
size_t HubLabelStore::getSpaceConsumption() {
    return 0;
};
}