#pragma once
#include "PathFinderBase.h"
#include "path_finder/graphs/CHGraph.h"
#include "path_finder/helper/Static.h"
#include "path_finder/routing/HubLabelCreator.h"
#include "path_finder/storage/CellIdStore.h"
#include "path_finder/storage/HubLabelStore.h"
#include <path_finder/helper/Stopwatch.h>
#include <queue>
#include <utility>
#include <vector>

namespace pathFinder {
/**
 * @brief
 * Hybrid path finder which combines hub labels and contraction hierarchies
 * to enable sub millisecond world scale shortest path queries.
 *
 * @tparam HubLabelStore
 * @tparam Graph
 * @tparam CellIdStore
 */
class HybridPathFinder : public PathFinderBase {
private:
  typedef std::vector<CostNode> costNodeVec_t;

  std::shared_ptr<HubLabelStore> m_hubLabelStore;
  std::shared_ptr<CHGraph> m_graph;
  std::shared_ptr<CellIdStore> m_cellIdStore;
  Level m_labelsUntilLevel;
  bool m_hubLabelsCalculated;
  bool m_cellIdsCalculated;
  std::vector<Distance> m_cost;
  std::vector<NodeId> m_visited;

public:
  /**
   * @brief
   * Initialize stores, graph, level, cost array.
   * @param hubLabelStore store with computed hubLabels
   * @param graph contraction hierarchy graph
   * @param cellIdStore store for egde -> oscar cell id
   * @param labelsUntilLevel level until the labels are computed in the store
   */
  HybridPathFinder(std::shared_ptr<HubLabelStore> hubLabelStore, const std::shared_ptr<CHGraph> &graph,
                   std::shared_ptr<CellIdStore> cellIdStore, Level labelsUntilLevel, bool hubLabelsCalculated = false,
                   bool cellIdsCalculated = false)
      : m_hubLabelStore(std::move(hubLabelStore)), m_graph(graph), m_cellIdStore(std::move(cellIdStore)),
        m_labelsUntilLevel(labelsUntilLevel), m_hubLabelsCalculated(hubLabelsCalculated),
        m_cellIdsCalculated(cellIdsCalculated) {
    m_cost.reserve(graph->getNumberOfNodes());
    while (m_cost.size() < graph->getNumberOfNodes())
      m_cost.push_back(MAX_DISTANCE);
  }

  /**
   * @brief
   * Computes the shortest path and distance from one node id to another.
   * @details
   * Does a contraction hierarchy query until the level for which the labels are
   * precomputed. Then merges those found labels to get the path and distance.
   * Also finds all the oscar cells which are visited while traversing the path.
   * @param source node id of the source node
   * @param target node id of the target node
   * @return RoutingResult object with path, distance, cell ids
   */
  RoutingResult getShortestPath(NodeId source, NodeId target) override;

  /**
   * @brief
   * Computes the shortest path and distance from one coordinate to another.
   * @details
   * Finds the corresponding node ids for the coordinates.
   * Does a contraction hierarchy query until the level for which the labels are
   * precomputed. Then merges those found labels to get the path and distance.
   * Also finds all the oscar cells which are visited while traversing the path.
   * @param source coordinates of the source node
   * @param target coordinates of the target node
   * @param distance the distance will be stored in this variable
   * @param cellIds the oscar cell ids of the path will be stored in this
   * container
   * @return RoutingResult object with path, distance, cell ids
   */
  RoutingResult getShortestPath(LatLng source, LatLng target) override;
  size_t graphNodeSize();
  Level labelsUntilLevel();
  std::shared_ptr<CHGraph> getGraph();
  Level getMaxLevel();
  void setLabelsUntilLevel(Level level);

  /**
   * @brief
   * Calculates the label for a given source node id and direction.
   * @details
   * Does a ch query search until all labels which need to be fetched are known
   * and merges those labels to dynamicaly generate a hub label.
   * If the label is already pre computed it just returns that label.
   * @param source node id of the node where the search starts
   * @param direction direction in which to search (either use the forward
   * (normal) edges or backward (reverse) edges)
   * @return returns a vector which contains the label elements [nodeId, cost,
   * previousNode]
   */
  costNodeVec_t calcLabelHybrid(NodeId source, EdgeDirection direction, CalcLabelTimingInfo &calcLabelTimingInfo);

  /**
   * @brief
   * returns edge path from a node id path
   * @param path node id path
   * @param direction
   * @return edge path vector
   */
  std::vector<CHEdge> getEdgeVectorFromNodeIdPath(const std::vector<NodeId> &path, EdgeDirection direction);

  /**
   * @brief
   * add oscar cell ids which correspond to the given edge
   * @param edge needs to be in forward order
   * @param result cell id vector in which the cell ids will be added
   */
  void addCellIds(const CHEdge &edge, std::vector<CellId_t> &result);

  /**
   * @brief
   * Traverses the previous node entries in the label and finds the path.
   * @param label
   * @param topNode
   * @param sourceId
   * @return vector with nodeId visited in the path
   */
  std::vector<NodeId> getShortestPathFromLabel(const costNodeVec_t &label, NodeId topNode, NodeId sourceId);

  /**
   * finds the element given an id in a label
   * @param nodeId
   * @param label
   * @return the found element
   */
  std::optional<CostNode> findElementInLabel(NodeId nodeId, const costNodeVec_t &label);
};
} // namespace pathFinder
