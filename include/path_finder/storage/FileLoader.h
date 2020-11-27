#pragma once
#include "path_finder/routing/CHDijkstra.h"
#include "path_finder/routing/HybridPathFinder.h"
#include <path_finder/routing/HybridPathFinderBundle.h>
namespace pathFinder {

class FileLoader {
public:
  static auto loadHubLabelBundle(const std::string &bundleFolder) -> std::shared_ptr<HybridPathFinderBundle>;
  static auto loadHubLabelsShared(const std::string &configFolder) -> std::shared_ptr<HybridPathFinder>;
  static auto loadCHDijkstraShared(const std::string &configFolder) -> std::shared_ptr<CHDijkstra>;
  static auto loadGraph(const std::string &graphFolder) -> std::shared_ptr<CHGraph>;
  static auto loadCellIds(const std::string &cellIdFolder) -> std::shared_ptr<CellIdStore>;
  static auto loadHubLabels(const std::string &hubLabelFolder) -> std::shared_ptr<HubLabelStore>;
};
} // namespace pathFinder
