//
// Created by sokol on 14.05.20.
//

#ifndef MASTER_ARBEIT_OSCARINTEGRATION_H
#define MASTER_ARBEIT_OSCARINTEGRATION_H
#include "CHGraph.h"
namespace pathFinder{
class CellIdDiskWriter {
public:
  void operator()(const NodeId i, const std::vector<unsigned int>& cellIds) {
    for(auto id : cellIds)
      std::cout << id << std::endl;
  }
};
class OscarIntegrator {
public:
  template<typename GeoPoint, typename Graph, typename CellIdsForEdge, typename DiskWriter>
  static void writeCellIdsForEdges(Graph& graph, CellIdsForEdge cellIdsForEdge, DiskWriter diskWriter) {
    for(int i = 0; i < graph.edges.size(); ++i) {
      const auto& edge = graph.edges[i];
      const auto sourceNode = graph.getNode(edge.source);
      const auto targetNode = graph.getNode(edge.target);
      GeoPoint sourcePoint;
      sourcePoint.lat() = sourceNode.latLng.lat;
      sourcePoint.lon() = sourceNode.latLng.lng;
      GeoPoint targetPoint;
      targetPoint.lat() = targetNode.latLng.lat;
      targetPoint.lon() = targetNode.latLng.lng;

      auto cellIds = cellIdsForEdge(sourcePoint, targetPoint);
      diskWriter(i, cellIds);
    }
  }
};
}

#endif // MASTER_ARBEIT_OSCARINTEGRATION_H
