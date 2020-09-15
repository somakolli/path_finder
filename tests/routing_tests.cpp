#include <filesystem>
#include <gtest/gtest.h>
#include <path_finder/graphs/Graph.h>
#include <path_finder/routing/CHDijkstra.h>
#include <path_finder/routing/Dijkstra.h>
#include <path_finder/routing/HubLabelCreator.h>
#include <path_finder/routing/HybridPathFinder.h>
#include <path_finder/storage/GraphReader.h>
namespace pathFinder {
TEST(RoutingTest, DistanceWorks) {
  std::string path = "/home/sokol/Uni/path_finder/test-data/";
  Graph graph;
  GraphReader::readFmiFile(graph, path + "test.fmi");
  std::shared_ptr<CHGraph> chGraph = std::make_shared<CHGraph>();
  GraphReader::readCHFmiFile(chGraph, path + "test.chfmi", false);
  CHDijkstra chDijkstra(chGraph);
  auto hubLabelStore = HubLabelStore::makeShared(chGraph->getNodes().size());
  HubLabelCreator hlc(chGraph, hubLabelStore);
  int labelsUntilLevel = 0;
  hlc.create(labelsUntilLevel);

  CellIdStore ramCellIdStore(chGraph->getNumberOfEdges());
  HybridPathFinder hybridPathFinder(hubLabelStore, chGraph, std::make_shared<CellIdStore>(ramCellIdStore),
                                    labelsUntilLevel);

  Dijkstra dijkstra(graph);
  for (int i = 0; i < graph.numberOfNodes; ++i) {
    for (int j = 0; j < graph.numberOfNodes; ++j) {
      auto normalDistance = dijkstra.getShortestDistance(i, j);
      auto chDistance = chDijkstra.getShortestDistance(i, j);
      auto routingResult = hybridPathFinder.getShortestPath(i, j);
      ASSERT_EQ(normalDistance, routingResult.distance);
      ASSERT_EQ(normalDistance, chDistance.value());
    }
  }

  // ASSERT_EQ(22, graph.nodes.size());
}
TEST(RoutingTest, PathFindingWorks) {
  std::string path = "/home/sokol/Uni/path_finder/test-data/";
  Graph graph;
  GraphReader::readFmiFile(graph, path + "test.fmi");
  std::shared_ptr<CHGraph> chGraph = std::make_shared<CHGraph>();
  GraphReader::readCHFmiFile(chGraph, path + "test.chfmi", false);
  std::shared_ptr<HubLabelStore> hubLabelStore = HubLabelStore::makeShared(chGraph->getNumberOfNodes());
  HubLabelCreator hlc(chGraph, hubLabelStore);
  int labelsUntilLevel = 0;
  hlc.create(labelsUntilLevel);
  auto ramCellIdStore = std::make_shared<CellIdStore>(chGraph->getNumberOfEdges());
  HybridPathFinder hybridPathFinder(hubLabelStore, chGraph, ramCellIdStore, labelsUntilLevel);

  auto routingResult = hybridPathFinder.getShortestPath(0, 0);
  ASSERT_TRUE(routingResult.path.empty());
  auto routingResult2 = hybridPathFinder.getShortestPath(1, 3);
  ASSERT_EQ(routingResult2.path[0].lat, 1);
  ASSERT_EQ(routingResult2.path[1].lat, 2);
  ASSERT_EQ(routingResult2.path[2].lat, 3);
  auto routingResult3 = hybridPathFinder.getShortestPath(1, 20);
  ASSERT_EQ(routingResult3.path[0].lat, 1);
  ASSERT_EQ(routingResult3.path[1].lat, 2);
  ASSERT_EQ(routingResult3.path[2].lat, 3);
  ASSERT_EQ(routingResult3.path[3].lat, 4);
  ASSERT_EQ(routingResult3.path[4].lat, 5);
  ASSERT_EQ(routingResult3.path[5].lat, 11);
  ASSERT_EQ(routingResult3.path[6].lat, 17);
  ASSERT_EQ(routingResult3.path[7].lat, 20);
  auto routingResult4 = hybridPathFinder.getShortestPath(2, 19);

  ASSERT_EQ(22, graph.nodes.size());
}
} // namespace pathFinder
