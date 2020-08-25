#ifndef MASTER_ARBEIT_FILELOADER_H
#define MASTER_ARBEIT_FILELOADER_H
#include "path_finder/routing/HybridPathFinder.h"
#include "path_finder/routing/CHDijkstra.h"
namespace pathFinder{

class FileLoader {
public:
  static std::shared_ptr<HybridPathFinder> loadHubLabelsShared(const std::string& configFolder);
  static std::shared_ptr<CHDijkstra> loadCHDijkstraShared(const std::string& configFolder);
  static std::shared_ptr<CHGraph> loadGraph(const std::string& graphFolder);
  static std::shared_ptr<CellIdStore> loadCellIds(const std::string& cellIdFolder);
  static std::shared_ptr<HubLabelStore> loadHubLabels(const std::string& hubLabelFolder);
};
}

#endif // MASTER_ARBEIT_FILELOADER_H
