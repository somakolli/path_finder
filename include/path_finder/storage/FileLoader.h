#pragma once
#include "path_finder/routing/CHDijkstra.h"
#include "path_finder/routing/HybridPathFinder.h"
namespace pathFinder {

class FileLoader {
public:
  static auto loadHubLabelsShared(const std::string &configFolder) -> std::shared_ptr<HybridPathFinder>;
  static auto loadCHDijkstraShared(const std::string &configFolder) -> std::shared_ptr<CHDijkstra>;
  static auto loadGraph(const std::string &graphFolder) -> std::shared_ptr<CHGraph>;
  static auto loadCellIds(const std::string &cellIdFolder) -> std::shared_ptr<CellIdStore>;
  static auto loadHubLabels(const std::string &hubLabelFolder) -> std::shared_ptr<HubLabelStore>;
};
} // namespace pathFinder
