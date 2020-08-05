#include <fstream>
#include <path_finder/helper/Timer.h>
#include <path_finder/storage/FileLoader.h>
std::shared_ptr<pathFinder::FileLoader::HybridPF>
pathFinder::FileLoader::loadHubLabelsShared(const std::string &configFolder) {
  auto chGraph = loadGraph(configFolder + "/graph/");
  auto cellIdStore = loadCellIds(configFolder + "/cellIds/");
  auto hubLabelStore = loadHubLabels(configFolder + "/hubLabels/");

  return std::make_shared<HybridPF>(HybridPF(hubLabelStore, chGraph, cellIdStore, hubLabelStore->calculatedUntilLevel));
}
std::shared_ptr<pathFinder::MMapGraph> pathFinder::FileLoader::loadGraph(const std::string &graphFolder) {
  std::ifstream t(graphFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<GraphDataInfo>(str);
  auto nodes = Static::getFromFileMMap<CHNode>(config.nodes, graphFolder);
  auto forwardEdges = Static::getFromFileMMap<CHEdge>(config.forwardEdges, graphFolder);
  auto backwardEdges = Static::getFromFileMMap<CHEdge>(config.backwardEdges, graphFolder);
  auto forwardOffset = Static::getFromFileMMap<NodeId>(config.forwardOffset, graphFolder);
  auto backwardOffset = Static::getFromFileMMap<NodeId>(config.backwardOffset, graphFolder);
  auto chGraph = std::make_shared<MMapGraph>(CHGraph(nodes, forwardEdges, backwardEdges,
                                                     forwardOffset, backwardOffset, nodes.size()));
  // set up grid
  for(auto gridEntry : config.gridMapEntries) {
    chGraph->gridMap[gridEntry.latLng] = gridEntry.pointerPair;
  }
  return chGraph;
}
std::shared_ptr<pathFinder::MMapCellIdStore> pathFinder::FileLoader::loadCellIds(const std::string &cellIdFolder) {
  std::ifstream t(cellIdFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<CellDataInfo>(str);
  auto cellIds = Static::getFromFileMMap<CellId_t>(config.cellIds, cellIdFolder);
  auto cellIdsOffset = Static::getFromFileMMap<OffsetElement>(config.cellIdsOffset, cellIdFolder);
  auto cellIdStore = std::make_shared<MMapCellIdStore>(CellIdStore(cellIds, cellIdsOffset));
  return cellIdStore;
}
std::shared_ptr<pathFinder::MMapHubLabelStore> pathFinder::FileLoader::loadHubLabels(const std::string &hubLabelFolder) {

  std::ifstream t(hubLabelFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<HubLabelDataInfo>(str);
  auto forwardHublabels = Static::getFromFileMMap<CostNode>(config.forwardHublabels, hubLabelFolder);
  auto backwardHublabels = Static::getFromFileMMap<CostNode>(config.backwardHublabels, hubLabelFolder);
  auto forwardHublabelOffset = Static::getFromFileMMap<OffsetElement>(config.forwardHublabelOffset, hubLabelFolder);
  auto backwardHublabelOffset = Static::getFromFileMMap<OffsetElement>(config.backwardHublabelOffset, hubLabelFolder);
  return std::make_shared<MMapHubLabelStore>(HubLabelStore(forwardHublabels, backwardHublabels,
                                                           forwardHublabelOffset, backwardHublabelOffset,
                                                           config.calculatedUntilLevel));
}
