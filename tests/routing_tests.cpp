#include <filesystem>
#include <gtest/gtest.h>
#include <path_finder/Dijkstra.h>
#include <path_finder/Graph.h>
#include <path_finder/GraphReader.h>
#include <path_finder/HubLabelCreator.h>
#include <path_finder/HybridPathFinder.h>
namespace pathFinder {
TEST(RoutingTest, DistanceWorks) {
  std::string path = "../../../../vendor/path_finder/test-data/";
  for (const auto & entry : std::filesystem::directory_iterator(path))
    std::cerr << entry.path() << std::endl;
  Graph graph;
  GraphReader::readFmiFile(graph, path + "test.fmi");
  CHGraph chGraph;
  GraphReader::readCHFmiFile(chGraph, path + "test.chfmi", false);
  HubLabelStore<std::vector> hubLabelStore(chGraph.getNodes().size());
  HubLabelCreator hlc(chGraph, hubLabelStore);
  int labelsUntilLevel = 7;
  hlc.create(labelsUntilLevel);
  RamCellIdStore ramCellIdStore(chGraph.getForwardEdges().size());
  HybridPathFinder hybridPathFinder(std::make_shared<RamHubLabelStore>(hubLabelStore), std::make_shared<RamGraph>(chGraph), std::make_shared<RamCellIdStore>(ramCellIdStore), labelsUntilLevel);

  Dijkstra dijkstra(graph);
  for(int i = 0; i < graph.numberOfNodes; ++i) {
    for(int j = 0; j < graph.numberOfNodes; ++j) {
      auto normalDistance = dijkstra.getShortestDistance(i, j);
      auto routingResult = hybridPathFinder.getShortestPath(i, j);
      ASSERT_EQ(normalDistance, routingResult.distance);
    }
  }

  ASSERT_EQ(22, graph.nodes.size());
}
TEST(RoutingTest, PathFindingWorks) {
  std::string path = "../../../../vendor/path_finder/test-data/";
  for (const auto & entry : std::filesystem::directory_iterator(path))
    std::cerr << entry.path() << std::endl;
  Graph graph;
  GraphReader::readFmiFile(graph, path + "test.fmi");
  CHGraph chGraph;
  GraphReader::readCHFmiFile(chGraph, path + "test.chfmi", false);
  HubLabelStore<std::vector> hubLabelStore(chGraph.getNodes().size());
  HubLabelCreator hlc(chGraph, hubLabelStore);
  int labelsUntilLevel = 0;
  hlc.create(labelsUntilLevel);
  RamCellIdStore ramCellIdStore(chGraph.getForwardEdges().size());
  HybridPathFinder hybridPathFinder(std::make_shared<RamHubLabelStore>(hubLabelStore), std::make_shared<RamGraph>(chGraph), std::make_shared<RamCellIdStore>(ramCellIdStore), labelsUntilLevel);

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
}
