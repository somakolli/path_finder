//
// Created by sokol on 14.05.20.
//

#ifndef MASTER_ARBEIT_OSCARINTEGRATION_H
#define MASTER_ARBEIT_OSCARINTEGRATION_H
#include "path_finder/storage/CellIdStore.h"
#include <path_finder/graphs/CHGraph.h>
#include <algorithm>
#include <iostream>
#include <mutex>
#include <execution>
namespace pathFinder{
class CellIdDiskWriter {
private:
  CellIdStore& _cellIdStore;
public:
  explicit CellIdDiskWriter(CellIdStore& cellIdStore);
  void operator()(const NodeId i, const std::vector<unsigned int>& cellIds) {
    _cellIdStore.storeCellIds(i, cellIds);
  }
};
class OscarIntegrator {
public:
  template<typename GeoPoint, typename CellIdsForEdge, typename DiskWriter, typename KVStore>
  static void writeCellIdsForEdges(const CHGraph& graph, CellIdsForEdge cellIdsForEdge, DiskWriter diskWriter, KVStore& store) {
     const auto edges = graph.getEdges();
    int progress = 0;
#pragma omp parallel for default(none) shared(edges, graph, cellIdsForEdge, diskWriter, store, progress, std::cout)
    for(int i = 0; i < graph.getNumberOfEdges(); ++i) {
      const auto& edge = edges[i];
      std::vector<uint32_t> cellIds;
      std::vector<CHEdge> fullEdges;
      if(edge.child1.has_value()){
        fullEdges = graph.getPathFromShortcut(edge, 0);
      } else {
        fullEdges.emplace_back(edge);
      }
      for(const auto& edge: fullEdges) {
        const auto sourceNode = graph.getNode(edge.source);
        const auto targetNode = graph.getNode(edge.target);
        GeoPoint sourcePoint;
        sourcePoint.lat() = sourceNode.latLng.lat;
        sourcePoint.lon() = sourceNode.latLng.lng;
        GeoPoint targetPoint;
        targetPoint.lat() = targetNode.latLng.lat;
        targetPoint.lon() = targetNode.latLng.lng;

        try {
          auto cellIdsEdge = cellIdsForEdge(sourcePoint, targetPoint);
          cellIds.insert(cellIds.end(), cellIdsEdge.begin(), cellIdsEdge.end());
        } catch (std::exception& e) {

        }
      }

      cellIds.erase(std::remove(cellIds.begin(), cellIds.end(), 4294967295), cellIds.end());
 #pragma omp critical
      {
        diskWriter(i, cellIds);
        ++progress;
        if(progress % 1000 == 0)
            std::cout << "progress: " << progress << "/" << graph.getNumberOfEdges() << '\n';
        //std::cout << "count: " << cellIds.size() << '\n';
      }
    }
  }
};
} // namespace pathFinder

#endif // MASTER_ARBEIT_OSCARINTEGRATION_H
