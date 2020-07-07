#ifndef MASTER_ARBEIT_FILELOADER_H
#define MASTER_ARBEIT_FILELOADER_H
#include "HybridPathFinder.h"
namespace pathFinder{
class FileLoader {
public:
  using HybridPF = HybridPathFinder<HubLabelStore<MmapVector, MmapVector<OffsetElement>>,
                  CHGraph<MmapVector,
                               MmapVector<NodeId>>,
                       CellIdStore<MmapVector, MmapVector<OffsetElement>>>;
  static std::shared_ptr<HybridPF> loadHubLabelsShared(const std::string& configFolder);
};
}

#endif // MASTER_ARBEIT_FILELOADER_H
