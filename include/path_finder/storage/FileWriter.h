#ifndef MASTER_ARBEIT_FILEWRITER_H
#define MASTER_ARBEIT_FILEWRITER_H
#include "CellIdStore.h"
#include "DataConfig.h"
#include "HubLabelStore.h"
#include <path_finder/helper/Static.h>
#include <path_finder/helper/Types.h>
namespace pathFinder {
class FileWriter {
public:
  static void writeGraph(const RamGraph & graph, const std::string& graphName, const std::string& folder);
  static void writeHubLabels(const RamHubLabelStore & hubLabelStore, Level level, const std::string& graphName, const std::string& folder);
  static void writeCells(const RamCellIdStore& cellIdStore, const std::string& graphName, const std::string& folder);
  static void writeAll(RamGraph* graph, RamHubLabelStore* hubLabelStore,
                       RamCellIdStore* cellIdStore, const std::string &folder);
};
}
#endif // MASTER_ARBEIT_FILEWRITER_H
