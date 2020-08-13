#include <fstream>
#include <path_finder/helper/Timer.h>
#include <path_finder/storage/FileLoader.h>
std::shared_ptr<pathFinder::HybridPF>
pathFinder::FileLoader::loadHubLabelsShared(const std::string &configFolder) {
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

  return std::make_shared<HybridPF>(hubLabelStore, chGraph, cellIdStore, hubLabelStore->calculatedUntilLevel,
                                             config.hubLabelsCalculated, config.cellIdsCalculated);
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
  auto chGraph = std::make_shared<MMapGraph>(nodes, forwardEdges, backwardEdges,
                                                     forwardOffset, backwardOffset, nodes.size());
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
  auto cellIdStore = std::make_shared<MMapCellIdStore>(cellIds.data(), cellIds.size(),
                                                                   cellIdsOffset.data(), cellIdsOffset.size());
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

  HubLabelStoreInfo hubLabelStoreInfo{};
  hubLabelStoreInfo.numberOfLabels = forwardHublabelOffset.size();
  hubLabelStoreInfo.forwardLabelSize = forwardHublabels.size();
  hubLabelStoreInfo.backwardLabelSize = forwardHublabels.size();
  hubLabelStoreInfo.forwardLabels = forwardHublabels.data();
  hubLabelStoreInfo.backwardLabels = backwardHublabels.data();
  hubLabelStoreInfo.forwardOffset = forwardHublabelOffset.data();
  hubLabelStoreInfo.backwardOffset = backwardHublabelOffset.data();
  hubLabelStoreInfo.calculatedUntilLevel = config.calculatedUntilLevel;
  auto hls = std::make_shared<MMapHubLabelStore>(hubLabelStoreInfo);
  hls->maxLevel = config.maxLevel;
  hls->calculatedUntilLevel = config.calculatedUntilLevel;
  return hls;
}
std::shared_ptr<pathFinder::HybridPFRam>
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

  return std::make_shared<HybridPFRam>(hubLabelStore, chGraph, cellIdStore, hubLabelStore->calculatedUntilLevel,
                                             config.hubLabelsCalculated, config.cellIdsCalculated);
}
std::shared_ptr<pathFinder::RamGraph> pathFinder::FileLoader::loadGraphRam(const std::string &graphFolder) {
  std::ifstream t(graphFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<GraphDataInfo>(str);
  auto nodes = Static::getFromFile<CHNode>(config.nodes, graphFolder);
  auto forwardEdges = Static::getFromFile<CHEdge>(config.forwardEdges, graphFolder);
  auto backwardEdges = Static::getFromFile<CHEdge>(config.backwardEdges, graphFolder);
  auto forwardOffset = Static::getFromFile<NodeId>(config.forwardOffset, graphFolder);
  auto backwardOffset = Static::getFromFile<NodeId>(config.backwardOffset, graphFolder);
  auto chGraph = std::make_shared<RamGraph>(nodes, forwardEdges, backwardEdges,
                                                     forwardOffset, backwardOffset, nodes.size());
  // set up grid
  for(auto gridEntry : config.gridMapEntries) {
    chGraph->gridMap[gridEntry.latLng] = gridEntry.pointerPair;
  }
  return chGraph;
}
std::shared_ptr<pathFinder::RamCellIdStore> pathFinder::FileLoader::loadCellIdsRam(const std::string &cellIdFolder) {
  std::ifstream t(cellIdFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<CellDataInfo>(str);
  auto cellIds = Static::getFromFile<CellId_t>(config.cellIds, cellIdFolder);
  auto cellIdsOffset = Static::getFromFile<OffsetElement>(config.cellIdsOffset, cellIdFolder);
  auto cellIdStore = std::make_shared<RamCellIdStore>(CellIdStore(cellIds.data(), cellIds.size(),
                                                                  cellIdsOffset.data(), cellIdsOffset.size()));
  return cellIdStore;
}
std::shared_ptr<pathFinder::RamHubLabelStore> pathFinder::FileLoader::loadHubLabelsRam(const std::string &hubLabelFolder) {

  std::ifstream t(hubLabelFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<HubLabelDataInfo>(str);
  auto forwardHublabels = Static::getFromFilePointer<CostNode>(config.forwardHublabels, hubLabelFolder);
  auto backwardHublabels = Static::getFromFilePointer<CostNode>(config.backwardHublabels, hubLabelFolder);
  auto forwardHublabelOffset = Static::getFromFilePointer<OffsetElement>(config.forwardHublabelOffset, hubLabelFolder);
  auto backwardHublabelOffset = Static::getFromFilePointer<OffsetElement>(config.backwardHublabelOffset, hubLabelFolder);

  HubLabelStoreInfo hubLabelStoreInfo{};
  hubLabelStoreInfo.numberOfLabels = config.forwardHublabelOffset.size;
  hubLabelStoreInfo.forwardLabelSize = config.forwardHublabels.size;
  hubLabelStoreInfo.backwardLabelSize = config.backwardHublabels.size;
  hubLabelStoreInfo.forwardLabels = forwardHublabels;
  hubLabelStoreInfo.backwardLabels = backwardHublabels;
  hubLabelStoreInfo.forwardOffset = forwardHublabelOffset;
  hubLabelStoreInfo.backwardOffset = backwardHublabelOffset;
  hubLabelStoreInfo.calculatedUntilLevel = config.calculatedUntilLevel;

  auto hls =  std::make_shared<RamHubLabelStore>(hubLabelStoreInfo);
  hls->maxLevel = config.maxLevel;
  return hls;
}
