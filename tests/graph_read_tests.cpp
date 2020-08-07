#include <filesystem>
#include <gtest/gtest.h>
#include <path_finder/graphs/Graph.h>
#include <path_finder/storage/FileLoader.h>
#include <path_finder/storage/FileWriter.h>
#include <path_finder/storage/GraphReader.h>
#include <ctime>

namespace pathFinder {
namespace GraphTest {
const std::string PATH = "/home/sokol/Uni/path_finder/test-data/";

template <typename Edges>
bool edgeExists(const pathFinder::RamGraph & graph, Edges edges, LatLng source, LatLng target) {
  for(const auto& edge : edges) {
    const auto& source1 = graph.getNodes()[edge.source];
    const auto& target1 = graph.getNodes()[edge.target];
    if(source == source1.latLng && target == target1.latLng)
      return true;
  }
  return false;
}



bool haveSameEdges(pathFinder::RamGraph & graph, CHNode node,
                   pathFinder::RamGraph & reorderedGraph, CHNode reorderedNode) {
  const auto& edges = graph.edgesFor(node.id, EdgeDirection::FORWARD);
  const auto& reorderEdges = reorderedGraph.edgesFor(reorderedNode.id, EdgeDirection::FORWARD);
  for(const auto& edge : edges) {
    const auto& source = graph.getNodes()[edge.source];
    const auto& target = graph.getNodes()[edge.target];
    if(!edgeExists(reorderedGraph, reorderEdges, source.latLng, target.latLng))
      return false;
  }
  return true;
}

void randomizeLatLngs(RamGraph& graph) {
  srand (static_cast <unsigned> (time(nullptr)));

  for(auto& node : graph.getNodes()) {
    auto LOlat = 45;
    auto HIlat = 46;
    auto lat = LOlat + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HIlat-LOlat)));
    auto LOlng = 8;
    auto HIlng = 9;
    auto lng = LOlng + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HIlng-LOlng)));
    node.latLng = LatLng{lat, lng};
  }
}


TEST(Graph, ReadWorks) {
  Graph graph;
  std::cout << "Current path is " << std::filesystem::current_path() << '\n';
  GraphReader::readFmiFile(graph, PATH + "test.fmi");
  auto edges = graph.edgesFor(1);
  auto edge0 = Edge(*edges.begin());
  auto edge1 = Edge(*(edges.begin() + 1));
  auto edge2 = Edge(*(edges.begin() + 2));
  ASSERT_EQ(edge0.source, 1);
  ASSERT_EQ(edge0.target, 0);
  ASSERT_EQ(edge1.source, 1);
  ASSERT_EQ(edge1.target, 2);
  ASSERT_EQ(edge2.source, 1);
  ASSERT_EQ(edge2.target, 7);
}
TEST(CHGraph, ReadWorks) {
  CHGraph graph;
  GraphReader::readCHFmiFile(graph, PATH + "test.chfmi", false);
  auto edges = graph.edgesFor(1, EdgeDirection::FORWARD);
  auto edge0 = CHEdge(*edges.begin());
  auto edge1 = CHEdge(*(edges.begin() + 1));
  auto edge2 = CHEdge(*(edges.begin() + 2));
  auto edge3 = CHEdge(*(edges.begin() + 3));
  auto edge4 = CHEdge(*(edges.begin() + 4));
  auto edge5 = CHEdge(*(edges.begin() + 5));
  ASSERT_EQ(edge0.source, 1);
  ASSERT_EQ(edge0.target, 0);
  ASSERT_EQ(edge1.source, 1);
  ASSERT_EQ(edge1.target, 2);
  ASSERT_EQ(edge2.source, 1);
  ASSERT_EQ(edge2.target, 3);
  ASSERT_EQ(edge3.source, 1);
  ASSERT_EQ(edge3.target, 5);
  ASSERT_EQ(edge4.source, 1);
  ASSERT_EQ(edge4.target, 6);
  ASSERT_EQ(edge5.source, 1);
  ASSERT_EQ(edge5.target, 7);
}
TEST(CHGraph, ReadFromDiskWorks) {
  CHGraph graph;
  GraphReader::readCHFmiFile(graph, PATH + "test.chfmi", false);

  FileWriter::writeGraph(graph, "test", "testGraph/");
  auto graphLoaded = FileLoader::loadGraph("testGraph");
  auto edges = graphLoaded->edgesFor(1, EdgeDirection::FORWARD);
  auto edge0 = CHEdge(*edges.begin());
  auto edge1 = CHEdge(*(edges.begin() + 1));
  auto edge2 = CHEdge(*(edges.begin() + 2));
  auto edge3 = CHEdge(*(edges.begin() + 3));
  auto edge4 = CHEdge(*(edges.begin() + 4));
  auto edge5 = CHEdge(*(edges.begin() + 5));
  ASSERT_EQ(edge0.source, 1);
  ASSERT_EQ(edge0.target, 0);
  ASSERT_EQ(edge1.source, 1);
  ASSERT_EQ(edge1.target, 2);
  ASSERT_EQ(edge2.source, 1);
  ASSERT_EQ(edge2.target, 3);
  ASSERT_EQ(edge3.source, 1);
  ASSERT_EQ(edge3.target, 5);
  ASSERT_EQ(edge4.source, 1);
  ASSERT_EQ(edge4.target, 6);
  ASSERT_EQ(edge5.source, 1);
  ASSERT_EQ(edge5.target, 7);

}
TEST(CHGraph, GridReorderWorks) {
  CHGraph graph;
  GraphReader::readCHFmiFile(graph, PATH + "test.chfmi", false);
  randomizeLatLngs(graph);

  std::vector<std::pair<LatLng, LatLng>> forwardEdgesBeforeReorder;
  std::vector<std::pair<LatLng, LatLng>> backwardEdgesBeforeReorder;

  for(const auto& edge :graph.m_edges) {
    const auto& sourceNode = graph.getNodes()[edge.source];
    const auto& targetNode = graph.getNodes()[edge.target];
    forwardEdgesBeforeReorder.emplace_back(sourceNode.latLng, targetNode.latLng);
  }
  for(const auto& edge :graph.getBackEdges()) {
    const auto& sourceNode = graph.getNodes()[edge.source];
    const auto& targetNode = graph.getNodes()[edge.target];
    backwardEdgesBeforeReorder.emplace_back(sourceNode.latLng, targetNode.latLng);
  }
  CHGraph reorderedGraph;
  GraphReader::readCHFmiFile(reorderedGraph, PATH + "test.chfmi", true);
  for(const auto& node : graph.getNodes()) {
    auto reorderedNode = reorderedGraph.getNode(reorderedGraph.getNodeIdFor(node.latLng));
    ASSERT_TRUE(haveSameEdges(graph, node, reorderedGraph, reorderedNode));
  }


  for(const auto& [source, target]: forwardEdgesBeforeReorder) {
      ASSERT_TRUE(edgeExists(graph, reorderedGraph.m_edges, source, target));
  }
  for(const auto& [source, target]: backwardEdgesBeforeReorder) {
      ASSERT_TRUE(edgeExists(graph, reorderedGraph.getBackEdges(), source, target));
  }



}
}
}