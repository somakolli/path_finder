//
// Created by sokol on 29.01.20.
//

#ifndef MASTER_ARBEIT_STATIC_H
#define MASTER_ARBEIT_STATIC_H
#include "DataConfig.h"
#include "MmapVector.h"
#include <any>
#include <cstddef>
#include <iostream>
#include <sys/mman.h>
#include <vector>

namespace pathFinder {
/**
 * @brief
 * Static function container
 */
class Static {
public:
  /**
   * @brief
   * Iterates through two iterators, merges them and stores the result in a container.
   *
   * TODO: document replacePrevious
   * @tparam ItA
   * @tparam ItB
   * @tparam Container
   * @tparam Distance
   * @tparam ReplacePrevious
   * @param aBegin
   * @param aEnd
   * @param bBegin
   * @param bEnd
   * @param distanceToLabel
   * @param result
   * @param replacePrevious
   */
  template <typename ItA, typename ItB, typename Container, typename Distance, typename ReplacePrevious>
  static inline void merge(ItA aBegin, ItA aEnd, ItB bBegin, ItB bEnd,
                           Distance distanceToLabel, Container &result,
                           ReplacePrevious replacePrevious) {
    auto i = aBegin;
    auto j = bBegin;
    while (i < aEnd && j < bEnd) {
      if (i->id < j->id) {
        result.push_back(replacePrevious(*i));
        ++i;
      } else if (j->id < i->id) {
        result.emplace_back(
            replacePrevious(j->id, j->cost + distanceToLabel, j->previousNode)
            );
        ++j;
      } else {
        if (i->cost < j->cost + distanceToLabel)
          result.emplace_back(replacePrevious(*i));
        else
          result.emplace_back(replacePrevious(j->id, j->cost + distanceToLabel,
                              j->previousNode));
        ++j;
        ++i;
      }
    }
    while (i != aEnd) {
      result.push_back(replacePrevious(*i));
      ++i;
    }
    while (j != bEnd) {
      result.emplace_back(replacePrevious(j->id, j->cost + distanceToLabel, j->previousNode));
      ++j;
    }
  }

  template <typename T>
  static inline void writeVectorToFile(std::vector<T> vector,
                                       const char *filename) {
    FILE *file = fopen64(filename, "w+");
    if (fwrite(vector.data(), sizeof(T), vector.size(), file) == 0)
      std::cerr << "could not write file " << filename << std::endl;
    fflush(file);
  }

  template <typename T>
  static inline std::vector<T>
  getFromFile(pathFinder::BinaryFileDescription fileDescription,
              const std::string& folderPrefix) {
    std::string path = folderPrefix  + '/'+ fileDescription.path;
    FILE *file = fopen64(
        path.c_str(), "r");
    std::vector<T> buf(fileDescription.size);
    std::fread(&buf[0], sizeof(buf[0]), buf.size(), file);
    return buf;
  }
  template <typename T>
  static inline auto
  getFromFileMMap(pathFinder::BinaryFileDescription fileDescription,
                  const std::string& folderPrefix) {
    return pathFinder::MmapVector<T>(
        std::string(folderPrefix + '/' + fileDescription.path).c_str(),
        fileDescription.size);
  }
  /**
  * @brief
  * sorts a label by node ids
  * @param label input label to be store
  */
  static inline void sortLabel(std::vector<CostNode> &label) {
    std::sort(label.begin(), label.end(),
              [](const CostNode &node1, const CostNode &node2) {
                return node1.id == node2.id ? node1.cost < node2.cost
                                            : node1.id < node2.id;
              });
  }
  /**
   * @brief
   * calculates the shortest distance of two labels
   *
   * @details
   * This function finds the entry which is present in both labels and
   * the added cost is the lowest.
   * The node id of that entry is store in topNode.
   *
   * @param forwardLabels iterator for the forward labels
   * @param backwardLabels iterator for the backward labels
   * @param topNode store for node id of the topNode (see details)
   * @return
   */
  static inline std::optional<pathFinder::Distance> getShortestDistance(
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
  template <typename Edge>
  static inline void reverseEdge(Edge& edge) {
    auto copyEdge = edge;
    edge.source = copyEdge.target;
    edge.target = copyEdge.source;
  }
};
} // namespace pathFinder

#endif // MASTER_ARBEIT_STATIC_H
