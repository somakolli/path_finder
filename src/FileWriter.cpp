#include <fstream>
#include <path_finder/storage/FileWriter.h>
namespace pathFinder{
void FileWriter::writeGraph(const CHGraph &graph, const std::string& graphName, const std::string& folder) {
  std::string command = "mkdir " + folder;
  system(command.c_str());
  pathFinder::GraphDataInfo dataConfig;
  dataConfig.graphName = graphName;
  dataConfig.timestamp = Static::getTimeStampString();
  dataConfig.nodes = {"nodes", graph.m_numberOfNodes, true};
  dataConfig.forwardEdges = {"forwardEdges", graph.m_numberOfEdges, true};
  dataConfig.forwardOffset = {"forwardOffset", graph.m_numberOfNodes + 1, false};
  dataConfig.backwardEdges = {"backwardEdges", graph.m_numberOfEdges, true};
  dataConfig.backwardOffset = {"backwardOffset", graph.m_numberOfNodes + 1, false};
  Static::writeVectorToFile(graph.m_nodes, graph.m_numberOfNodes, (folder + dataConfig.nodes.path).c_str());
  Static::writeVectorToFile(graph.m_edges, graph.m_numberOfEdges, (folder + dataConfig.forwardEdges.path).c_str());
  Static::writeVectorToFile(graph.m_offset, graph.m_numberOfNodes + 1, (folder + dataConfig.forwardOffset.path).c_str());
  Static::writeVectorToFile(graph.m_backEdges, graph.m_numberOfEdges, (folder + dataConfig.backwardEdges.path).c_str());
  Static::writeVectorToFile(graph.m_backOffset, graph.m_numberOfNodes + 1, (folder + dataConfig.backwardOffset.path).c_str());
  //write config to file
  std::ofstream out(folder + "/config.json");
  nlohmann::json j;
  to_json(j, dataConfig);
  std::string jsonString = j.dump();
  out << j.dump();
  out.close();
}
void FileWriter::writeHubLabels(const HubLabelStore &hubLabelStore, const std::string& graphName, const std::string& folder) {
  std::string command = "mkdir " + folder;
  system(command.c_str());
  pathFinder::HubLabelDataInfo dataConfig;
  dataConfig.graphName = graphName;
  dataConfig.timestamp = Static::getTimeStampString();
  dataConfig.forwardHublabels = {"forwardHubLabels", hubLabelStore.getForwardLabelsSize(), true};
  dataConfig.backwardHublabels = { "backwardHubLabels", hubLabelStore.getBackwardLabelsSize(), true};
  dataConfig.forwardHublabelOffset = { "forwardHubLabelOffset", hubLabelStore.getForwardOffsetSize(), false};
  dataConfig.backwardHublabelOffset = {"backwardHubLabelOffset", hubLabelStore.getBackwardOffsetSize(), false};
  dataConfig.maxLevel = hubLabelStore.maxLevel;
  dataConfig.calculatedUntilLevel = hubLabelStore.calculatedUntilLevel;
  // write hub label files
  Static::writeVectorToFile(hubLabelStore.m_forwardLabels, hubLabelStore.m_forwardLabelSize,
                            (folder + dataConfig.forwardHublabels.path).c_str());
  Static::writeVectorToFile(hubLabelStore.m_backwardLabels, hubLabelStore.m_backwardLabelSize,
                            (folder + dataConfig.backwardHublabels.path).c_str());
  Static::writeVectorToFile(hubLabelStore.m_forwardOffset, hubLabelStore.m_numberOfLabels,
                            (folder + dataConfig.forwardHublabelOffset.path).c_str());
  Static::writeVectorToFile(hubLabelStore.m_backwardOffset, hubLabelStore.m_numberOfLabels,
                            (folder + dataConfig.backwardHublabelOffset.path).c_str());

  //write config to file
  std::ofstream out(folder + "/config.json");
  nlohmann::json j;
  to_json(j, dataConfig);
  std::string jsonString = j.dump();
  out << j.dump();
  out.close();
}
void FileWriter::writeCells(const CellIdStore &cellIdStore, const std::string &graphName,
                            const std::string &folder) {
  std::string command = "mkdir " + folder;
  system(command.c_str());
  pathFinder::CellDataInfo dataConfig;
  dataConfig.graphName = graphName;
  dataConfig.timestamp = Static::getTimeStampString();
  dataConfig.cellIds = {"cellIds", cellIdStore.cellIdSize()};
  dataConfig.cellIdsOffset = {"cellIdsOffset", cellIdStore.offsetSize()};
  Static::writeVectorToFile(cellIdStore._cellIds, cellIdStore._cellIdSize,(folder + dataConfig.cellIds.path).c_str());
  Static::writeVectorToFile(cellIdStore._offsetVector, cellIdStore._offsetVectorSize, (folder + dataConfig.cellIdsOffset.path).c_str());
  //write config to file
  std::ofstream out(folder + "/config.json");
  nlohmann::json j;
  to_json(j, dataConfig);
  std::string jsonString = j.dump();
  out << j.dump();
  out.close();
}
void FileWriter::writeAll(std::shared_ptr<CHGraph> graph, std::shared_ptr<HubLabelStore> hubLabelStore,
                          std::shared_ptr<CellIdStore> cellIdStore, const std::string &folder) {
  std::string command = "mkdir " + folder;
  system(command.c_str());
  pathFinder::HybridPfDataInfo dataInfo;
  if(graph != nullptr) {
    dataInfo.graphFolder = "graph/";
    writeGraph(*graph, "stgt", folder + '/' + dataInfo.graphFolder);
  }

  if(hubLabelStore != nullptr) {
    dataInfo.hubLabelFolder = "hubLabels/";
    writeHubLabels(*hubLabelStore, "stgt", folder + '/' + dataInfo.hubLabelFolder);
    dataInfo.hubLabelsCalculated = true;
    dataInfo.labelsUntilLevel = hubLabelStore->calculatedUntilLevel;
  }

  if(cellIdStore != nullptr){
    dataInfo.cellIdFolder = "cellIds/";
    writeCells(*cellIdStore, "stgt", folder + '/' + dataInfo.cellIdFolder);
    dataInfo.cellIdsCalculated = true;
  }
  //write config to file
  std::ofstream out(folder + "/config.json");
  nlohmann::json j;
  to_json(j, dataInfo);
  std::string jsonString = j.dump();
  out << j.dump();
  out.close();
}
}
