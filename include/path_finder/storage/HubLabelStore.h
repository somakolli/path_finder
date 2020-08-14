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
};
class HubLabelStore {
private:
  CostNode* forwardLabels;
  CostNode* backwardLabels;
  OffsetElement* forwardOffset;
  OffsetElement* backwardOffset;
  size_t forwardLabelSize = 0;
  size_t backwardLabelSize = 0;
  size_t numberOfLabels;
  bool _mmap = false;
  std::string _folder = "";
public:
  size_t calculatedUntilLevel;
  size_t maxLevel;
  explicit HubLabelStore(size_t numberOfLabels, bool mmap = false, std::string folder = "hubLabels");
  static std::shared_ptr<HubLabelStore> makeShared(size_t numberOfLabels);
  ~HubLabelStore();
  explicit HubLabelStore(HubLabelStoreInfo hubLabelStoreInfo);

  void store(const std::vector<CostNode> &label, NodeId id,
             EdgeDirection direction);

  void retrieve(NodeId id, EdgeDirection direction, std::vector<CostNode>& storeVec);
  void retrieve(NodeId id, EdgeDirection direction, CostNode *&storeVec, size_t& size);
  size_t getSize(NodeId, EdgeDirection direction);
  size_t getNumberOfLabels() const;
  CostNode* getForwardLabels();
  CostNode* getForwardLabels() const;
  size_t getForwardLabelsSize() const;
  CostNode* getBackwardLabels();
  CostNode* getBackwardLabels() const;
  size_t getBackwardLabelsSize() const;
  OffsetElement* getForwardOffset();
  OffsetElement* getForwardOffset() const;
  size_t getForwardOffsetSize() const;
  OffsetElement* getBackwardOffset();
  OffsetElement* getBackwardOffset() const;
  size_t getBackwardOffsetSize() const;
  size_t getSpaceConsumption();

  std::string printStore();
};
} // namespace pathFinder

#endif // MASTER_ARBEIT_HUBLABELSTORE_H
