#include <filesystem>
#include <gtest/gtest.h>
#include <path_finder/graphs/Graph.h>
#include <path_finder/storage/FileLoader.h>
#include <path_finder/storage/FileWriter.h>
#include <path_finder/storage/GraphReader.h>

namespace pathFinder {
namespace GraphTest {
const std::string PATH = "/home/sokol/Uni/master-arbeit/vendor/path_finder/test-data/";

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
TEST(CHGraph, GridWorks) {
  CHGraph graph;
  GraphReader::readCHFmiFile(graph, PATH + "test.chfmi", true);
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
}
}