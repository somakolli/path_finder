//
// Created by sokol on 14.05.20.
//

#ifndef MASTER_ARBEIT_OSCARINTEGRATION_H
#define MASTER_ARBEIT_OSCARINTEGRATION_H
#include "CHGraph.h"
#include "CellIdStore.h"
namespace pathFinder{
class CellIdDiskWriter {
private:
  CellIdStore<std::vector>& _cellIdStore;
public:
  CellIdDiskWriter(CellIdStore<std::vector>& cellIdStore);
  void operator()(const NodeId i, const std::vector<unsigned int>& cellIds) {
    _cellIdStore.storeCellIds(i, cellIds);
  }
};
class OscarIntegrator {
public:
  template<typename GeoPoint, typename Graph, typename CellIdsForEdge, typename DiskWriter>
  static void writeCellIdsForEdges(Graph& graph, CellIdsForEdge cellIdsForEdge, DiskWriter diskWriter) {
    int progress = 0;
#pragma omp parallel for
    for(int i = 0; i < graph.edges.size(); ++i) {
      const auto& edge = graph.edges[i];
      if(edge.child1.has_value()){
        std::cout << "skip" << '\n';
        continue;
      }
      const auto sourceNode = graph.getNode(edge.source);
      const auto targetNode = graph.getNode(edge.target);
      GeoPoint sourcePoint;
      sourcePoint.lat() = sourceNode.latLng.lat;
      sourcePoint.lon() = sourceNode.latLng.lng;
      GeoPoint targetPoint;
      targetPoint.lat() = targetNode.latLng.lat;
      targetPoint.lon() = targetNode.latLng.lng;

      auto cellIds = cellIdsForEdge(sourcePoint, targetPoint);
#pragma omp critical
      {
        diskWriter(i, cellIds);
        std::cout << "progress: " << progress++ << "/" << graph.edges.size() << '\n';
      }
    }
  }
};
}

#endif // MASTER_ARBEIT_OSCARINTEGRATION_H
