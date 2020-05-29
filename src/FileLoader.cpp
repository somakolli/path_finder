#include <path_finder/FileLoader.h>
pathFinder::FileLoader::HL
pathFinder::FileLoader::loadHubLabels(const std::string &configFolder) {
  std::ifstream t(configFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile(str);
  auto nodes = Static::getFromFileMMap<CHNode>(config.nodes, configFolder);
  auto forwardEdges = Static::getFromFileMMap<CHEdge>(config.forwardEdges, configFolder);
  auto backwardEdges = Static::getFromFileMMap<CHEdge>(config.backwardEdges, configFolder);
  auto forwardOffset = Static::getFromFileMMap<NodeId>(config.forwardOffset, configFolder);
  auto backwardOffset = Static::getFromFileMMap<NodeId>(config.backwardOffset, configFolder);
  auto forwardHublabels = Static::getFromFileMMap<CostNode>(config.forwardHublabels, configFolder);
  auto backwardHublabels = Static::getFromFileMMap<CostNode>(config.backwardHublabels, configFolder);
  auto forwardHublabelOffset = Static::getFromFileMMap<OffsetElement>(config.forwardHublabelOffset, configFolder);
  auto backwardHublabelOffset = Static::getFromFileMMap<OffsetElement>(config.backwardHublabelOffset, configFolder);
  auto cellIds = Static::getFromFileMMap<CellId_t>(config.cellIds, configFolder);
  auto cellIdsOffset = Static::getFromFileMMap<OffsetElement>(config.cellIdsOffset, configFolder);
  CHGraph chGraph(nodes, forwardEdges, backwardEdges, forwardOffset, backwardOffset, config.numberOfNodes);
  auto cellIdStore = CellIdStore(cellIds, cellIdsOffset);
  // set up grid
  for(auto gridEntry : config.gridMapEntries) {
    chGraph.gridMap[gridEntry.latLng] = gridEntry.pointerPair;
  }

  pathFinder::HubLabelStore hubLabelStore(forwardHublabels, backwardHublabels, forwardHublabelOffset, backwardHublabelOffset);
  pathFinder::Timer timer;
  return HL(chGraph, config.calculatedUntilLevel, hubLabelStore, timer, cellIdStore);
}
