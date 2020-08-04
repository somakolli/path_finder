#include <fstream>
#include <path_finder/helper/Timer.h>
#include <path_finder/storage/FileLoader.h>
std::shared_ptr<pathFinder::FileLoader::HybridPF>
pathFinder::FileLoader::loadHubLabelsShared(const std::string &configFolder) {
  std::ifstream t(configFolder + "/config.json");
  std::string str((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
  auto config = pathFinder::DataConfig::getFromFile<DataConfig>(str);
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
  auto chGraph = std::make_shared<MMapGraph>(CHGraph(nodes, forwardEdges, backwardEdges, forwardOffset, backwardOffset, config.numberOfNodes));
  auto cellIdStore = std::make_shared<MMapCellIdStore>(CellIdStore(cellIds, cellIdsOffset));
  // set up grid
  for(auto gridEntry : config.gridMapEntries) {
    chGraph->gridMap[gridEntry.latLng] = gridEntry.pointerPair;
  }
  std::cout << chGraph.use_count() << '\n';
  auto hubLabelStore = std::make_shared<MMapHubLabelStore>(HubLabelStore(forwardHublabels, backwardHublabels, forwardHublabelOffset, backwardHublabelOffset));
  pathFinder::Timer timer;
  return std::make_shared<HybridPF>(HybridPF(hubLabelStore, chGraph, cellIdStore, config.calculatedUntilLevel));
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
  return std::make_shared<MMapGraph>(CHGraph(nodes, forwardEdges, backwardEdges, forwardOffset, backwardOffset, nodes.size()));
}
