#ifndef MASTER_ARBEIT_FILELOADER_H
#define MASTER_ARBEIT_FILELOADER_H
#include "HubLabels.h"
namespace pathFinder{
class FileLoader {
public:
  using HL = HubLabels<HubLabelStore<MmapVector, MmapVector<OffsetElement>>,
                  CHGraph<MmapVector,
                               MmapVector<NodeId>>,
                       CellIdStore<MmapVector, MmapVector<OffsetElement>>>;
  static std::shared_ptr<HL> loadHubLabelsShared(const std::string& configFolder);
};
}

#endif // MASTER_ARBEIT_FILELOADER_H
