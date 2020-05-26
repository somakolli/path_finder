//
// Created by sokol on 14.05.20.
//

#ifndef MASTER_ARBEIT_OSCARINTEGRATION_H
#define MASTER_ARBEIT_OSCARINTEGRATION_H
#include "CHGraph.h"
namespace pathFinder{
class OscarIntegrator {
  template<typename Edges, typename CellIdsForEdge, typename DiskWriter>
  void writeCellIdsForEdges(const Edges& edges) {
    for(int i = 0; i < edges.size(); ++i) {
      const auto& edge = edges[i];
      auto cellIds = CellIdsForEdge(edge.source.lat, edge.source.lng, edge.target.lat, edge.target.lng);
      DiskWriter(i, cellIds);
    }
  }
};
}

#endif // MASTER_ARBEIT_OSCARINTEGRATION_H
