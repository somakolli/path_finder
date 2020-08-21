#ifndef MASTER_ARBEIT_FILEWRITER_H
#define MASTER_ARBEIT_FILEWRITER_H
#include "CellIdStore.h"
#include "DataConfig.h"
#include "HubLabelStore.h"
#include <path_finder/helper/Static.h>
#include <path_finder/helper/Types.h>
#include <path_finder/graphs/CHGraph.h>
namespace pathFinder {
class FileWriter {
public:
  static void writeGraph(const CHGraph & graph, const std::string& graphName, const std::string& folder);
  static void writeHubLabels(const HubLabelStore & hubLabelStore, const std::string& graphName, const std::string& folder);
  static void writeCells(const CellIdStore& cellIdStore, const std::string& graphName, const std::string& folder);
  static void writeAll(std::shared_ptr<CHGraph> graph, std::shared_ptr<HubLabelStore> hubLabelStore,
                       std::shared_ptr<CellIdStore> cellIdStore, const std::string &folder);
};
}
#endif // MASTER_ARBEIT_FILEWRITER_H
