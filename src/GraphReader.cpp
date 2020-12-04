//
// Created by sokol on 02.10.19.
//
#include "path_finder/storage/GraphReader.h"
#include "path_finder/graphs/Grid.h"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <path_finder/helper/Static.h>

void pathFinder::GraphReader::readFmiFile(pathFinder::Graph &graph, const std::string &filepath) {
  uint32_t numberOfEdges{};
  std::ifstream in(filepath);
  std::string line;
  // ignore first 5 lines
  for (auto i = 0; i < 5; ++i) {
    std::getline(in, line);
  }
  in >> graph.numberOfNodes;
  in >> numberOfEdges;
  uint32_t type;
  uint32_t maxSpeed;
  int i = graph.numberOfNodes + 1;
  Node node{};
  long id2;
  double elevation;
  while (--i > 0 && in >> node.id >> id2 >> node.latLng.lat >> node.latLng.lng >> elevation) {
    graph.nodes.push_back(node);
  }
  i = numberOfEdges + 1;
  Edge edge{};
  while (--i > 0 && in >> edge.source >> edge.target >> edge.distance >> type >> maxSpeed) {
    graph.edges.push_back(edge);
  }
  buildOffset(graph.edges.begin().base(), graph.edges.size(), graph.nodes.size(), graph.offset);
}

void pathFinder::GraphReader::readCHFmiFile(std::shared_ptr<pathFinder::CHGraph> graph, const std::string &filepath,
                                            bool reorderWithGrid) {
  std::ifstream in(filepath);
  std::string line;
  // ignore first 10 lines
  for (auto i = 0; i < 10; ++i) {
    std::getline(in, line);
  }
  in >> graph->m_numberOfNodes;
  in >> graph->m_numberOfEdges;
  
  if (graph->m_numberOfNodes >= std::numeric_limits<NodeId>::max()) {
	throw std::runtime_error("Graph has too many nodes");
  }

  if (graph->m_numberOfEdges >= std::numeric_limits<EdgeId>::max()) {
	throw std::runtime_error("Graph has too many edges");
  }
  
  std::cout << "Graph has " << graph->m_numberOfNodes << " nodes and " << graph->m_numberOfEdges << " edges" << std::endl;
  
  graph->m_nodes = new CHNode[graph->m_numberOfNodes];
  graph->m_edges = new CHEdge[graph->m_numberOfEdges];

  uint32_t type;
  uint32_t maxSpeed;
  EdgeId child1;
  EdgeId child2;
  uint64_t osmId;
  uint32_t el;
  std::size_t i{0};
  std::cout << "Reading nodes..." << std::endl;
  for(i=0; i < graph->m_numberOfNodes && in.good(); ++i) {
	CHNode & node =  graph->m_nodes[i];
	in >> node.id >> osmId >> node.latLng.lat >> node.latLng.lng >> el >> node.level;
	if (i%10000 == 0) {
		std::cout << '\xd' << i << "/" << graph->m_numberOfNodes << "=" << double(i)/graph->m_numberOfNodes*100 << "%" << std::flush;
	}
	if (node.id != i) {
		throw std::runtime_error("Node at position " + std::to_string(i) + " has invalid id");
	}
  }
  std::cout << std::endl;
  if (i != graph->m_numberOfNodes) {
	 throw std::runtime_error("Could not read all nodes. Got " + std::to_string(i)); 
  }
  
  std::cout << "Reading edges..." << std::endl;
  for(i=0; i < graph->m_numberOfEdges && in.good(); ++i) {
	CHEdge & edge = graph->m_edges[i];
	in >> edge.source >> edge.target >> edge.distance >> type >> maxSpeed >> child1 >> child2;
	if (child1 != EdgeId(-1)) {
		edge.child1 = std::make_optional(child1);
	}
	if (child2 != EdgeId(-1)) {
		edge.child2 = std::make_optional(child2);
	}
	if (i%10000 == 0) {
		std::cout << '\xd' << i << "/" << graph->m_numberOfEdges << "=" << double(i)/graph->m_numberOfEdges*100 << "%" << std::flush;
	}
  }
  std::cout << std::endl;
  if (i != graph->m_numberOfEdges) {
	throw std::runtime_error("Could not read all edges. Got " + std::to_string(i)); 
  }
  if (reorderWithGrid) {
    createGridForGraph(*graph, 10, 10);
  }
  calcBoundingBox(*graph);
#if TEST
  graph->randomizeLatLngs();
#endif
  graph->sortEdges();
  buildOffset(graph->m_edges, graph->m_numberOfEdges, graph->m_numberOfNodes, graph->m_offset);
  buildBackEdges(graph->m_edges, graph->m_backEdges, graph->m_numberOfEdges);
  buildOffset(graph->m_backEdges, graph->m_numberOfEdges, graph->m_numberOfNodes, graph->m_backOffset);
}

void pathFinder::GraphReader::sortEdges(MyIterator<CHEdge *> edges) {
  std::sort(edges.begin(), edges.end(), [](const auto &edge1, const auto &edge2) -> bool {
    return (edge1.source == edge2.source) ? edge1.target <= edge2.target : edge1.source < edge2.source;
  });
}
void pathFinder::GraphReader::buildOffset(const pathFinder::CHEdge *edges, size_t edgeSize, size_t nodeSize, NodeId *&offset) {
  if (edgeSize == 0)
    return;
  std::size_t offsetSize = nodeSize + 1;
  delete offset;
  offset = new NodeId[offsetSize];
  for(std::size_t i(0); i < offsetSize; ++i) {
	offset[i] = std::numeric_limits<NodeId>::max(); 
  }
  offset[nodeSize] = edgeSize; //last one points to one beyond
  //Edges are sorted according to source
  for(int64_t i = edgeSize - 1; i >= 0; --i) {
	//Edge with the smallest position is the last one that writes to the position at edges[i].source
    offset[edges[i].source] = i;
  }
  offset[0] = 0;
  offset[offsetSize-1] = edgeSize;
  //We now have offsets for each node that has edges.
  //Check for nodes without edges and set their offsets accordingly:
  //They point to the same position as the first valid offset AFTER them
  //Hence we iterate the offset vector in reverse and fix the wrong offsets
  //We have the invariant, that every offset with index larger than i is valid
  std::size_t nodesWithoutEdges = 0;
  for(uint64_t i(offsetSize-2); i > 1; --i) {
    if (offset[i] == std::numeric_limits<NodeId>::max()) {
      ++nodesWithoutEdges;
      offset[i] = offset[i+1];
    }
  }
  if (nodesWithoutEdges) {
    std::cerr << "Found " << nodesWithoutEdges << " nodes without edges" << std::endl;
  }
}
void pathFinder::GraphReader::buildBackEdges(const pathFinder::CHEdge *forwardEdges, pathFinder::CHEdge *&backEdges,
                                             size_t numberOfEdges) {
  delete backEdges;
  backEdges = new CHEdge[numberOfEdges];
  for(std::size_t i = 0; i < numberOfEdges; ++i) {
    backEdges[i].source = forwardEdges[i].target;
    backEdges[i].target = forwardEdges[i].source;
    backEdges[i].distance = forwardEdges[i].distance;
    backEdges[i].child1 = forwardEdges[i].child1;
    backEdges[i].child2 = forwardEdges[i].child2;
  }
  sortEdges(MyIterator(backEdges, backEdges + numberOfEdges));
}
void pathFinder::GraphReader::buildOffset(const pathFinder::Edge *edges, size_t edgeSize, size_t nodeSize, std::vector<NodeId> &offset) {
  if (edgeSize == 0)
    return;

  NodeId numberOfNodes = edges[edgeSize - 1].source + 1;
  assert(numberOfNodes == nodeSize); //BUG: this is verly likely false
  offset.reserve(numberOfNodes + 1);
  for (int64_t i = 0; i < numberOfNodes + 1; ++i) {
    offset.emplace_back(0);
  }
  size_t offsetSize = numberOfNodes + 1;
  offset[numberOfNodes] = edgeSize;
  for (int64_t i = edgeSize - 1; i >= 0; --i) {
    offset[edges[i].source] = i;
  }
  for (int64_t i = edges[0].source + 1; i < offsetSize - 2; ++i) {

    if (offset[i] == 0) {
      size_t j = i + 1;
      while (offset[j] == 0) {
        ++j;
      }
      size_t offsetToSet = offset[j];
      --j;
      size_t firstNullPosition = i;
      while (j >= firstNullPosition) {
        offset[j] = offsetToSet;
        --j;
        ++i;
      }
    }
  }
  offset[0] = 0;
  offset[offsetSize] = edgeSize;
}
void pathFinder::GraphReader::createGridForGraph(pathFinder::CHGraph &graph, double latStretchFactor,
                                                 double lngStretchFactor) {

  auto grid = std::make_shared<Grid>(latStretchFactor, lngStretchFactor);
  std::map<std::pair<int, int>, std::vector<NodeId>> map;

  //Compute mapping grid bin -> nodes
  for(std::size_t i(0); i < graph.m_numberOfNodes; ++i) {
    map[grid->getKeyFor(graph.getNode(i).latLng)].emplace_back(i);
  }
  std::vector<NodeId> oldIdToNewId(graph.m_numberOfNodes, std::numeric_limits<NodeId>::max());
  NodeId newNodeId = 0;
  for (auto const & [positionPair, oldNodeIds] : map) {
	NodeId gridBinBegin = newNodeId;
    for (NodeId oldNodeId : oldNodeIds) {
      oldIdToNewId.at(oldNodeId) = newNodeId;
	  ++newNodeId;
    }
    (*grid)[positionPair] = std::make_pair(gridBinBegin, newNodeId);
  }
  assert(newNodeId == graph.m_numberOfNodes);
  //now sort the nodes according to oldIdToNewId
  std::sort(graph.m_nodes, graph.m_nodes+graph.m_numberOfNodes, [&](auto && first, auto && second) -> bool {
	return oldIdToNewId.at(first.id) < oldIdToNewId.at(second.id);
  });
  //set correct node ids
  for(NodeId i(0); i < graph.m_numberOfNodes; ++i) {
	graph.m_nodes[i].id = i;
  }
  //Fix edges
  for(EdgeId i(0); i < graph.m_numberOfEdges; ++i) {
	auto & edge = graph.m_edges[i];
    edge.source = oldIdToNewId.at(edge.source);
    edge.target = oldIdToNewId.at(edge.target);
  }
  graph.grid = grid;
}
void pathFinder::GraphReader::calcBoundingBox(pathFinder::CHGraph &graph) {
  BoundingBox boundingBox{
      graph.getNode(0).latLng.lat, // north
      graph.getNode(0).latLng.lng, // east
      graph.getNode(0).latLng.lat, // south
      graph.getNode(0).latLng.lng, // west
  };
  for (const auto &node : graph.getNodes()) {
    auto lat = node.latLng.lat;
    auto lng = node.latLng.lng;
    if (lat > boundingBox.north)
      boundingBox.north = lat;
    if (lat < boundingBox.south)
      boundingBox.south = lat;
    if (lng > boundingBox.east)
      boundingBox.east = lng;
    if (lng < boundingBox.west)
      boundingBox.west = lng;
  }
  graph.boundingBox = boundingBox;

  graph.midPoint = boundingBox.calcMidPoint(boundingBox.southWest(), boundingBox.northEast());
}
