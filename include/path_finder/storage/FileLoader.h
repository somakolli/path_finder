#ifndef MASTER_ARBEIT_FILELOADER_H
#define MASTER_ARBEIT_FILELOADER_H
#include "path_finder/routing/HybridPathFinder.h"
namespace pathFinder{
using HybridPF = HybridPathFinder<MMapGraph,MMapCellIdStore>;
using HybridPFRam = HybridPathFinder<RamGraph,RamCellIdStore>;
class FileLoader {
public:
  static std::shared_ptr<HybridPF> loadHubLabelsShared(const std::string& configFolder);
  static std::shared_ptr<MMapGraph> loadGraph(const std::string& graphFolder);
  static std::shared_ptr<MMapCellIdStore> loadCellIds(const std::string& cellIdFolder);
  static std::shared_ptr<MMapHubLabelStore> loadHubLabels(const std::string& hubLabelFolder);
  static std::shared_ptr<HybridPFRam> loadHubLabelsSharedRam(const std::string& configFolder);
  static std::shared_ptr<RamGraph> loadGraphRam(const std::string& graphFolder);
  static std::shared_ptr<RamCellIdStore> loadCellIdsRam(const std::string& cellIdFolder);
  static std::shared_ptr<RamHubLabelStore> loadHubLabelsRam(const std::string& hubLabelFolder);
};
}

#endif // MASTER_ARBEIT_FILELOADER_H
