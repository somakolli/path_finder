#ifndef MASTER_ARBEIT_FILELOADER_H
#define MASTER_ARBEIT_FILELOADER_H
#include "path_finder/routing/HybridPathFinder.h"
#include "path_finder/routing/CHDijkstra.h"
namespace pathFinder{

class FileLoader {
public:
  static std::shared_ptr<HybridPathFinder> loadHubLabelsShared(const std::string& configFolder, bool mmap = true);
  static std::shared_ptr<CHDijkstra> loadCHDijkstraShared(const std::string& configFolder, bool mmap = true);
  static std::shared_ptr<MMapGraph> loadGraph(const std::string& graphFolder, bool mmap = true);
  static std::shared_ptr<MMapCellIdStore> loadCellIds(const std::string& cellIdFolder, bool mmap = true);
  static std::shared_ptr<MMapHubLabelStore> loadHubLabels(const std::string& hubLabelFolder, bool mmap = true);
  static std::shared_ptr<HybridPathFinder> loadHubLabelsSharedRam(const std::string& configFolder);
  static std::shared_ptr<CHDijkstra> loadCHDijkstraRam(const std::string& configFolder);
  static std::shared_ptr<RamGraph> loadGraphRam(const std::string& graphFolder);
  static std::shared_ptr<RamCellIdStore> loadCellIdsRam(const std::string& cellIdFolder);
  static std::shared_ptr<RamHubLabelStore> loadHubLabelsRam(const std::string& hubLabelFolder);
};
}

#endif // MASTER_ARBEIT_FILELOADER_H
