//
// Created by sokol on 23.01.20.
//

#ifndef MASTER_ARBEIT_HUBLABELSTORE_H
#define MASTER_ARBEIT_HUBLABELSTORE_H

#include "path_finder/graphs/Graph.h"
#include <vector>
#include <memory>
namespace pathFinder {
struct OffsetElement {
  size_t position = 0;
  size_t size = 0;
};
struct HubLabelStoreInfo {
  CostNode* forwardLabels;
  CostNode* backwardLabels;
  OffsetElement* forwardOffset;
  OffsetElement* backwardOffset;
  size_t forwardLabelSize;
  size_t backwardLabelSize;
  size_t numberOfLabels;
  size_t calculatedUntilLevel;
  bool forwardLabelsMMap = false;
  bool backwardLabelsMMap = false;
  bool forwardOffsetMMap = false;
  bool backwardOffsetMMap = false;
  void setAllMMap(bool condition) {
    forwardLabelsMMap = condition;
    backwardLabelsMMap = condition;
    forwardOffsetMMap = condition;
    backwardOffsetMMap = condition;
  }
};
class HubLabelStore {
  friend class FileWriter;
private:
  CostNode* m_forwardLabels;
  CostNode* m_backwardLabels;
  OffsetElement* m_forwardOffset;
  OffsetElement* m_backwardOffset;
  size_t m_forwardLabelSize = 0;
  size_t m_backwardLabelSize = 0;
  size_t m_numberOfLabels = 0;
  bool m_forwardLabelsMMap = false;
  bool m_backwardLabelsMMap = false;
  bool m_forwardOffsetMMap = false;
  bool m_backwardOffsetMMap = false;
public:
  size_t calculatedUntilLevel = 0;
  size_t maxLevel = 0;
  explicit HubLabelStore(size_t numberOfLabels);
  static std::shared_ptr<HubLabelStore> makeShared(size_t numberOfLabels);
  ~HubLabelStore();
  explicit HubLabelStore(HubLabelStoreInfo hubLabelStoreInfo);

  void store(const std::vector<CostNode> &label, NodeId id,
             EdgeDirection direction);

  void retrieve(NodeId id, EdgeDirection direction, std::vector<CostNode>& storeVec);
  void retrieve(NodeId id, EdgeDirection direction, CostNode *&storeVec, size_t& size);
  [[nodiscard]] size_t getNumberOfLabels() const;
  [[nodiscard]] size_t getForwardLabelsSize() const;
  [[nodiscard]] size_t getBackwardLabelsSize() const;
  [[nodiscard]] size_t getForwardOffsetSize() const;
  [[nodiscard]] size_t getBackwardOffsetSize() const;
  static size_t getSpaceConsumption();
  std::string printStore();
};
} // namespace pathFinder

#endif // MASTER_ARBEIT_HUBLABELSTORE_H
