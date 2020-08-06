//
// Created by sokol on 14.05.20.
//

#ifndef MASTER_ARBEIT_OSCARINTEGRATION_H
#define MASTER_ARBEIT_OSCARINTEGRATION_H
#include "../../../../liboscar/vendor/sserialize/include/sserialize/utility/exceptions.h"
#include "path_finder/graphs/CHGraph.h"
#include "path_finder/storage/CellIdStore.h"
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
  template<typename GeoPoint, typename Graph, typename CellIdsForEdge, typename DiskWriter, typename Store>
  static void writeCellIdsForEdges(Graph& graph, CellIdsForEdge cellIdsForEdge, DiskWriter diskWriter, Store store) {
    int progress = 0;
//#pragma omp parallel for
    for(int i = 0; i < graph.m_edges.size(); ++i) {
      const auto& edge = graph.m_edges[i];
      if(edge.child1.has_value()){
        ++progress;
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
      std::vector<uint32_t > cellIds;
      try {
        cellIds = cellIdsForEdge(sourcePoint, targetPoint);
      } catch (sserialize::OutOfBoundsException& e) {
        
      }
      cellIds.erase(std::remove(cellIds.begin(), cellIds.end(), 4294967295), cellIds.end());
 //#pragma omp critical
      {
        diskWriter(i, cellIds);
        ++progress;
        if(progress % 1000 == 0)
            std::cout << "progress: " << progress << "/" << graph.m_edges.size() << '\n';
        //std::cout << "count: " << cellIds.size() << '\n';
      }
    }
  }
};
} // namespace pathFinder

#endif // MASTER_ARBEIT_OSCARINTEGRATION_H
