#include <fstream>
#include <path_finder/routing/CHDijkstra.h>
#include <path_finder/storage/FileLoader.h>
namespace pathFinder {
auto
pathFinder::FileLoader::loadHubLabelsShared(const std::string &configFolder) -> std::shared_ptr<pathFinder::HybridPathFinder> {

  const std::string& configFilePath = configFolder + "/config.json";
  std::ifstream t(configFilePath);
  if(t.bad() || t.fail())
    throw std::runtime_error("could not open configfile" + configFilePath);
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  if(str == "") {
    throw std::runtime_error("config file bad " + configFilePath);
  }
  auto config = pathFinder::DataConfig::getFromFile<HybridPfDataInfo>(str);
  auto chGraph = loadGraph(configFolder + "/graph/");
  std::shared_ptr<HubLabelStore> hubLabelStore;
  std::shared_ptr<CellIdStore> cellIdStore;
  if (config.cellIdsCalculated) {
    cellIdStore = loadCellIds(configFolder + "/cellIds/");
  }
  if (config.hubLabelsCalculated)
    hubLabelStore = loadHubLabels(configFolder + "/hubLabels");

  return std::make_shared<HybridPathFinder>(hubLabelStore, chGraph, cellIdStore, hubLabelStore->calculatedUntilLevel,
                                            config.hubLabelsCalculated, config.cellIdsCalculated);
}
auto pathFinder::FileLoader::loadGraph(const std::string &graphFolder) -> std::shared_ptr<pathFinder::CHGraph> {
  std::ifstream t(graphFolder + "/config.json");
  if(t.fail() || t.bad())
    throw std::runtime_error("could not find graph config. path: " + graphFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<GraphDataInfo>(str);
  auto nodes = Static::getFromFile<CHNode>(config.nodes, graphFolder, config.nodes.mmap);
  auto forwardEdges = Static::getFromFile<CHEdge>(config.forwardEdges, graphFolder, config.forwardEdges.mmap);
  auto backwardEdges = Static::getFromFile<CHEdge>(config.backwardEdges, graphFolder, config.backwardEdges.mmap);
  auto forwardOffset = Static::getFromFile<NodeId>(config.forwardOffset, graphFolder, config.forwardOffset.mmap);
  auto backwardOffset = Static::getFromFile<NodeId>(config.backwardOffset, graphFolder, config.backwardOffset.mmap);

  CHGraphCreateInfo chGraphCreateInfo{};
  chGraphCreateInfo.nodes = nodes;
  chGraphCreateInfo.edges = forwardEdges;
  chGraphCreateInfo.backEdges = backwardEdges;
  chGraphCreateInfo.offset = forwardOffset;
  chGraphCreateInfo.backOffset = backwardOffset;
  chGraphCreateInfo.numberOfEdges = config.forwardEdges.size;
  chGraphCreateInfo.numberOfNodes = config.nodes.size;

  chGraphCreateInfo.nodesMMap = config.nodes.mmap;
  chGraphCreateInfo.edgesMMap = config.forwardEdges.mmap;
  chGraphCreateInfo.backEdgesMMap = config.backwardEdges.mmap;
  chGraphCreateInfo.offsetMMap = config.forwardOffset.mmap;
  chGraphCreateInfo.backOffsetMMap = config.backwardOffset.mmap;
  chGraphCreateInfo.boundingBox = config.boundingBox;
  chGraphCreateInfo.midPoint = config.midPoint;

  // set up grid
  if (config.gridCalculated) {
    std::ifstream gridStream(graphFolder + config.gridMapFile);
    std::string gridStr((std::istreambuf_iterator<char>(gridStream)), std::istreambuf_iterator<char>());
    auto gridConfig = pathFinder::DataConfig::getFromFile<GridMapEntries>(gridStr);
    chGraphCreateInfo.grid = std::make_shared<Grid>(gridConfig.latStretchFactor, gridConfig.lngStretchFactor);
    for (auto gridEntry : gridConfig.gridMapEntries) {
      chGraphCreateInfo.grid->operator[](gridEntry.latLng) = gridEntry.pointerPair;
    }
  }
  auto chGraph = std::make_shared<CHGraph>(chGraphCreateInfo);
  return chGraph;
}
auto pathFinder::FileLoader::loadCellIds(const std::string &cellIdFolder) -> std::shared_ptr<pathFinder::CellIdStore> {
  std::ifstream t(cellIdFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<CellDataInfo>(str);
  auto cellIds = Static::getFromFile<CellId_t>(config.cellIds, cellIdFolder, config.cellIds.mmap);
  auto cellIdsOffset = Static::getFromFile<OffsetElement>(config.cellIdsOffset, cellIdFolder, config.cellIds.mmap);

  CellIdStoreCreateInfo cellIdStoreCreateInfo{};
  cellIdStoreCreateInfo.cellIds = cellIds;
  cellIdStoreCreateInfo.offsetVector = cellIdsOffset;
  cellIdStoreCreateInfo.cellIdSize = config.cellIds.size;
  cellIdStoreCreateInfo.offsetSize = config.cellIdsOffset.size;

  cellIdStoreCreateInfo.cellIdsMMap = config.cellIds.mmap;
  cellIdStoreCreateInfo.offsetMMap = config.cellIdsOffset.mmap;

  auto cellIdStore = std::make_shared<CellIdStore>(cellIdStoreCreateInfo);
  return cellIdStore;
}
auto pathFinder::FileLoader::loadHubLabels(const std::string &hubLabelFolder) -> std::shared_ptr<pathFinder::HubLabelStore> {

  std::ifstream t(hubLabelFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<HubLabelDataInfo>(str);
  auto forwardHublabels =
      Static::getFromFile<CostNode>(config.forwardHublabels, hubLabelFolder, config.forwardHublabels.mmap);
  auto backwardHublabels =
      Static::getFromFile<CostNode>(config.backwardHublabels, hubLabelFolder, config.backwardHublabels.mmap);
  auto forwardHublabelOffset = Static::getFromFile<OffsetElement>(config.forwardHublabelOffset, hubLabelFolder,
                                                                  config.forwardHublabelOffset.mmap);
  auto backwardHublabelOffset = Static::getFromFile<OffsetElement>(config.backwardHublabelOffset, hubLabelFolder,
                                                                   config.backwardHublabelOffset.mmap);

  HubLabelStoreInfo hubLabelStoreInfo{};
  hubLabelStoreInfo.numberOfLabels = config.forwardHublabelOffset.size;
  hubLabelStoreInfo.forwardLabelSize = config.forwardHublabels.size;
  hubLabelStoreInfo.backwardLabelSize = config.backwardHublabels.size;
  hubLabelStoreInfo.forwardLabels = forwardHublabels;
  hubLabelStoreInfo.backwardLabels = backwardHublabels;
  hubLabelStoreInfo.forwardOffset = forwardHublabelOffset;
  hubLabelStoreInfo.backwardOffset = backwardHublabelOffset;
  hubLabelStoreInfo.calculatedUntilLevel = config.calculatedUntilLevel;

  hubLabelStoreInfo.forwardLabelsMMap = config.forwardHublabels.mmap;
  hubLabelStoreInfo.backwardLabelsMMap = config.backwardHublabels.mmap;
  hubLabelStoreInfo.forwardOffsetMMap = config.forwardHublabelOffset.mmap;
  hubLabelStoreInfo.backwardOffsetMMap = config.backwardHublabelOffset.mmap;

  auto hls = std::make_shared<HubLabelStore>(hubLabelStoreInfo);
  hls->maxLevel = config.maxLevel;
  hls->calculatedUntilLevel = config.calculatedUntilLevel;
  return hls;
}
auto FileLoader::loadCHDijkstraShared(const std::string &configFolder) -> std::shared_ptr<CHDijkstra> {
  auto graph = loadGraph(configFolder + "/graph/");
  return std::make_shared<CHDijkstra>(graph);
}
} // namespace pathFinder