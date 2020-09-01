#pragma once
#include "path_finder/storage/CellIdStore.h"
#include <algorithm>
#include <execution>
#include <iostream>
#include <mutex>
#include <path_finder/graphs/CHGraph.h>
namespace pathFinder {
class OscarIntegrator {
public:
  template <typename GeoPoint, typename CellIdsForEdge, typename KVStore>
  static void writeCellIdsForEdges(const CHGraph &graph, CellIdsForEdge cellIdsForEdge, CellIdStore &cellIdStore,
                                   KVStore &store) {
    const auto edges = graph.getEdges();
    int progress = 0;
#pragma omp parallel  for default(none) shared(edges, graph, cellIdsForEdge, cellIdStore, store, progress, std::cout) num_threads(16)
    for (int i = 0; i < graph.getNumberOfEdges(); ++i) {
      const auto &edge = edges[i];
      std::vector<uint32_t> cellIds;
      if (edge.child1.has_value()) {
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

      try {
        auto cellIdsEdge = cellIdsForEdge(sourcePoint, targetPoint);
        cellIds.insert(cellIds.end(), cellIdsEdge.begin(), cellIdsEdge.end());
      } catch (std::exception &e) {
      }
      cellIds.erase(std::remove(cellIds.begin(), cellIds.end(), 4294967295), cellIds.end());
#pragma omp critical
      {
        cellIdStore.storeCellIds(i, cellIds);
        ++progress;
        if (progress % 1000 == 0)
          std::cout << "progress: " << progress << "/" << graph.getNumberOfEdges() << '\n';
        // std::cout << "count: " << cellIds.size() << '\n';
      }
    }
    for (int i = 0; i < graph.getNumberOfEdges(); ++i) {
      const auto &edge = edges[i];
      if (edge.child1.has_value()) {
        const auto fullEdges = graph.getPathFromShortcut(edge, 0);
        std::vector<size_t> fullEdgeIds;
        fullEdgeIds.reserve(fullEdges.size());
        for (const auto fullEdge : fullEdges) {
          fullEdgeIds.emplace_back(graph.getEdgePosition(fullEdge, EdgeDirection::FORWARD).value());
        }
        auto fullCellIds = cellIdStore.getCellIds(fullEdgeIds);
        sort(fullCellIds.begin(), fullCellIds.end());
        (fullCellIds).erase(unique(fullCellIds.begin(), fullCellIds.end()), fullCellIds.end());
        cellIdStore.storeCellIds(i, fullCellIds);
      }
    }
    cellIdStore.shrink_to_fit();
  }
};
} // namespace pathFinder
