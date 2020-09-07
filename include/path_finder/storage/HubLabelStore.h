#pragma once

#include <path_finder/graphs/Graph.h>

#include <memory>
#include <vector>
namespace pathFinder {
struct HubLabelStoreInfo {
  CostNode *forwardLabels;
  CostNode *backwardLabels;
  OffsetElement *forwardOffset;
  OffsetElement *backwardOffset;
  size_t forwardLabelSize;
  size_t backwardLabelSize;
  size_t numberOfLabels;
  size_t calculatedUntilLevel;
  bool forwardLabelsMMap = false;
  bool backwardLabelsMMap = false;
  bool forwardOffsetMMap = false;
  bool backwardOffsetMMap = false;
};
class HubLabelStore {
  friend class FileWriter;

private:
  CostNode *m_forwardLabels;
  CostNode *m_backwardLabels;
  OffsetElement *m_forwardOffset;
  OffsetElement *m_backwardOffset;
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

  void store(const std::vector<CostNode> &label, NodeId id, EdgeDirection direction);
  using IdLabelPair = std::pair<NodeId, costNodeVec_t>;

  // idLabels vec will be deleted after store
  void store(std::vector<IdLabelPair> &idLabels, EdgeDirection direction);

  [[nodiscard]] MyIterator<const CostNode *> retrieveIt(NodeId id, EdgeDirection direction) const;

  [[nodiscard]] std::vector<CostNode> retrieve(NodeId id, EdgeDirection direction) const;
  void retrieve(NodeId id, EdgeDirection direction, CostNode *&storeVec, size_t &size) const;
  [[nodiscard]] size_t getNumberOfLabels() const;
  [[nodiscard]] size_t getForwardLabelsSize() const;
  [[nodiscard]] size_t getBackwardLabelsSize() const;
  [[nodiscard]] size_t getForwardOffsetSize() const;
  [[nodiscard]] size_t getBackwardOffsetSize() const;
  [[nodiscard]] auto getSpaceConsumption() const -> size_t;
  [[maybe_unused]] [[nodiscard]] std::string printStore() const;
};
} // namespace pathFinder
