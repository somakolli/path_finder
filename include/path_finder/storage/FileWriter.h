#pragma once
#include "CellIdStore.h"
#include "DataConfig.h"
#include "HubLabelStore.h"
#include <path_finder/graphs/CHGraph.h>
#include <path_finder/helper/Static.h>
#include <path_finder/helper/Types.h>
namespace pathFinder {
class FileWriter {
public:
  static void writeGraph(const CHGraph &graph, const std::string &graphName, const std::string &folder);
  static void writeHubLabels(const HubLabelStore &hubLabelStore, const std::string &graphName,
                             const std::string &folder);
  static void writeCells(const CellIdStore &cellIdStore, const std::string &graphName, const std::string &folder);
  static void writeAll(std::shared_ptr<CHGraph> graph, std::shared_ptr<HubLabelStore> hubLabelStore,
                       std::shared_ptr<CellIdStore> cellIdStore, const std::string &folder);
};
} // namespace pathFinder
