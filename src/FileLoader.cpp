#include <fstream>
#include <path_finder/helper/Timer.h>
#include <path_finder/storage/FileLoader.h>
std::shared_ptr<pathFinder::HybridPathFinder>
pathFinder::FileLoader::loadHubLabelsShared(const std::string &configFolder, bool mmap) {
  std::ifstream t(configFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<HybridPfDataInfo>(str);
  auto chGraph = loadGraph(configFolder + "/graph/");
  std::shared_ptr<MMapHubLabelStore> hubLabelStore;
  std::shared_ptr<MMapCellIdStore> cellIdStore;
  if(config.cellIdsCalculated){
    cellIdStore = loadCellIds(configFolder + "/cellIds/");
  }
  if(config.hubLabelsCalculated)
    hubLabelStore = loadHubLabels(configFolder + "/hubLabels/");

  return std::make_shared<HybridPathFinder>(hubLabelStore, chGraph, cellIdStore, hubLabelStore->calculatedUntilLevel,
                                             config.hubLabelsCalculated, config.cellIdsCalculated);
}
std::shared_ptr<pathFinder::CHGraph> pathFinder::FileLoader::loadGraph(const std::string &graphFolder, bool mmap) {
  std::ifstream t(graphFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<GraphDataInfo>(str);
  auto nodes = Static::getFromFile<CHNode>(config.nodes, graphFolder, mmap);
  auto forwardEdges = Static::getFromFile<CHEdge>(config.forwardEdges, graphFolder, mmap);
  auto backwardEdges = Static::getFromFile<CHEdge>(config.backwardEdges, graphFolder, mmap);
  auto forwardOffset = Static::getFromFile<NodeId>(config.forwardOffset, graphFolder, mmap);
  auto backwardOffset = Static::getFromFile<NodeId>(config.backwardOffset, graphFolder, mmap);

  CHGraphCreateInfo chGraphCreateInfo{};
  chGraphCreateInfo.nodes = nodes;
  chGraphCreateInfo.edges = forwardEdges;
  chGraphCreateInfo.backEdges = backwardEdges;
  chGraphCreateInfo.offset = forwardOffset;
  chGraphCreateInfo.backOffset = backwardOffset;
  chGraphCreateInfo.numberOfEdges = config.forwardEdges.size;
  chGraphCreateInfo.numberOfNodes = config.nodes.size;

  chGraphCreateInfo.setAllMMap(mmap);

  auto chGraph = std::make_shared<CHGraph>(chGraphCreateInfo);
  // set up grid
  for(auto gridEntry : config.gridMapEntries) {
    chGraph->gridMap[gridEntry.latLng] = gridEntry.pointerPair;
  }
  return chGraph;
}
std::shared_ptr<pathFinder::MMapCellIdStore> pathFinder::FileLoader::loadCellIds(const std::string &cellIdFolder, bool mmap) {
  std::ifstream t(cellIdFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<CellDataInfo>(str);
  auto cellIds = Static::getFromFile<CellId_t>(config.cellIds, cellIdFolder, mmap);
  auto cellIdsOffset = Static::getFromFile<OffsetElement>(config.cellIdsOffset, cellIdFolder, mmap);

  CellIdStoreCreateInfo cellIdStoreCreateInfo{};
  cellIdStoreCreateInfo.cellIds = cellIds;
  cellIdStoreCreateInfo.offsetVector = cellIdsOffset;
  cellIdStoreCreateInfo.cellIdSize = config.cellIds.size;
  cellIdStoreCreateInfo.offsetSize = config.cellIdsOffset.size;

  cellIdStoreCreateInfo.setAllMMap(mmap);

  auto cellIdStore = std::make_shared<CellIdStore>(cellIdStoreCreateInfo);
  return cellIdStore;
}
std::shared_ptr<pathFinder::MMapHubLabelStore> pathFinder::FileLoader::loadHubLabels(const std::string &hubLabelFolder, bool mmap) {

  std::ifstream t(hubLabelFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<HubLabelDataInfo>(str);
  auto forwardHublabels = Static::getFromFile<CostNode>(config.forwardHublabels, hubLabelFolder,mmap);
  auto backwardHublabels = Static::getFromFile<CostNode>(config.backwardHublabels, hubLabelFolder,mmap);
  auto forwardHublabelOffset = Static::getFromFile<OffsetElement>(config.forwardHublabelOffset, hubLabelFolder,mmap);
  auto backwardHublabelOffset = Static::getFromFile<OffsetElement>(config.backwardHublabelOffset, hubLabelFolder,mmap);

  HubLabelStoreInfo hubLabelStoreInfo{};
  hubLabelStoreInfo.numberOfLabels = config.forwardHublabelOffset.size;
  hubLabelStoreInfo.forwardLabelSize = config.forwardHublabels.size;
  hubLabelStoreInfo.backwardLabelSize = config.backwardHublabels.size;
  hubLabelStoreInfo.forwardLabels = forwardHublabels;
  hubLabelStoreInfo.backwardLabels = backwardHublabels;
  hubLabelStoreInfo.forwardOffset = forwardHublabelOffset;
  hubLabelStoreInfo.backwardOffset = backwardHublabelOffset;
  hubLabelStoreInfo.calculatedUntilLevel = config.calculatedUntilLevel;

  hubLabelStoreInfo.setAllMMap(mmap);

  auto hls = std::make_shared<MMapHubLabelStore>(hubLabelStoreInfo);
  hls->maxLevel = config.maxLevel;
  hls->calculatedUntilLevel = config.calculatedUntilLevel;
  return hls;
}
std::shared_ptr<pathFinder::HybridPathFinder>
pathFinder::FileLoader::loadHubLabelsSharedRam(const std::string &configFolder) {
  std::ifstream t(configFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<HybridPfDataInfo>(str);
  auto chGraph = loadGraphRam(configFolder + "/graph/");
  std::shared_ptr<RamHubLabelStore> hubLabelStore;
  std::shared_ptr<RamCellIdStore> cellIdStore;
  if(config.cellIdsCalculated){
    cellIdStore = loadCellIdsRam(configFolder + "/cellIds/");
  }
  if(config.hubLabelsCalculated)
    hubLabelStore = loadHubLabelsRam(configFolder + "/hubLabels/");

  return std::make_shared<HybridPathFinder>(hubLabelStore, chGraph, cellIdStore, hubLabelStore->calculatedUntilLevel,
                                             config.hubLabelsCalculated, config.cellIdsCalculated);
}
std::shared_ptr<pathFinder::RamGraph> pathFinder::FileLoader::loadGraphRam(const std::string &graphFolder) {
  return loadGraph(graphFolder, false);
}
std::shared_ptr<pathFinder::RamCellIdStore> pathFinder::FileLoader::loadCellIdsRam(const std::string &cellIdFolder) {
  return loadCellIds(cellIdFolder, false);
}
std::shared_ptr<pathFinder::RamHubLabelStore> pathFinder::FileLoader::loadHubLabelsRam(const std::string &hubLabelFolder) {
  return loadHubLabels(hubLabelFolder, false);
}
