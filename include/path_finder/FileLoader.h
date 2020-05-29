#ifndef MASTER_ARBEIT_FILELOADER_H
#define MASTER_ARBEIT_FILELOADER_H
#include "HubLabels.h"
namespace pathFinder{
class FileLoader {
  using HL = HubLabels<HubLabelStore<MmapVector, MmapVector<OffsetElement>>,
                  CHGraph<MmapVector,
                               MmapVector<NodeId>>,
                       CellIdStore<MmapVector, MmapVector<OffsetElement>>>;
  static HL loadHubLabels(const std::string& configFolder);
};
}

#endif // MASTER_ARBEIT_FILELOADER_H
