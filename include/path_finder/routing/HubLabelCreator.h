#pragma once
#include "path_finder/graphs/CHGraph.h"
#include "path_finder/helper/SpaceMeasurer.h"
#include "path_finder/helper/Types.h"
#include "path_finder/storage/CellIdStore.h"
#include "path_finder/storage/HubLabelStore.h"
#include <memory>
#include <mutex>
#include <vector>
namespace pathFinder {

/**
 * @brief
 * Calculates hub labels based on a contraction hierarchy graph and stores them
 * in a hub label store.
 */
class HubLabelCreator {
private:
  Level m_labelsUntilLevel = 0;
  std::vector<CHNode> m_sortedNodes;

public:
  std::shared_ptr<CHGraph> m_graph;
  std::shared_ptr<HubLabelStore> m_hubLabelStore;
  std::shared_ptr<SpaceMeasurer> spaceMeasurer;

  /**
   * @brief
   * Constructs class and sets graph and hubLabelStore
   *
   * @param graph input graph in ch form
   * @param hubLabelStore empty store to be filled will labels
   */
  HubLabelCreator(std::shared_ptr<CHGraph> graph, std::shared_ptr<HubLabelStore> hubLabelStore);
  /**
   * @brief
   * starts hub label creation
   */
  void create(Level untilLevel);

private:
  /**
   * @brief
   * starts construction of each same level node range
   *
   * @details
   * The labels will be calculated from to bottom.
   * The labels for the nodes with the highest level will be computed first.
   * Labels for the nodes with the same level can be computed in parallel
   *
   * @param sameLevelRanges vector with ranges as pairs
   * @param maxLevel the maximum level of the graph
   * @param labelsUntilLevel dictates until which level the labels should be
   *                         calculated
   */
  void constructAllLabels(const std::vector<std::pair<uint32_t, uint32_t>> &sameLevelRanges, Level maxLevel,
                          Level labelsUntilLevel);
  /**
   * @brief
   * starts hub label calculation for one range
   *
   * @details
   * Since all nodes in the range have the same level each label can be
   * computed in parallel.
   * However they need to be stored in order and two threads cannot store at
   * the same time.
   *
   * @param range pair of ranges [begin, end] in the sortedNodes vector
   * @param edgeDirection calculate labels in either the forward or backward
   *                      direction
   */
  void processRangeParallel(const std::pair<uint32_t, uint32_t> &range, EdgeDirection edgeDirection);
  /**
   * @brief
   * calculates the hub label for one id
   *
   * @param id node id for which the label will be computed
   * @param direction direction for which the label will be computed
   * @return the computed label
   */
  [[nodiscard]] auto calcLabel(NodeId id, EdgeDirection direction) const -> std::vector<CostNode>;
  /**
   * @brief
   * removes not shortest distances in labels
   *
   * @details
   * Not every distance stored in a label is the shortest.
   * Not shortest distances are not needed and consume space.
   * This function checks each entry in a label if the set distance is the
   * shortest and removes it if not.
   *
   * @param label label to be pruned
   * @param nodeId id of the node corresponding to the label
   * @param direction edge direction of the hub label
   */
  void selfPrune(std::vector<CostNode> &label, NodeId nodeId, EdgeDirection direction) const;
  /**
   * @brief
   * calculates the shortest distance between two nodes if the labels are already
   * in the store
   *
   * @details
   * This function finds the entry which is present in both labels and
   * the added cost is the lowest.
   *
   * @param source node id
   * @param target node id
   * @return shortest distance value and nullopt if shortest distance is not known
   */
  auto getShortestDistance(NodeId source, NodeId target) -> std::optional<pathFinder::Distance>;
};
} // namespace pathFinder