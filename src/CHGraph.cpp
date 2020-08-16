//
// Created by sokol on 03.10.19.
//

#include <path_finder/graphs/CHGraph.h>
#include <path_finder/helper/Static.h>
#include <sys/mman.h>
pathFinder::CHGraph::CHGraph(pathFinder::CHGraphCreateInfo chGraphCreateInfo) {
  m_nodes = chGraphCreateInfo.nodes;
  m_edges = chGraphCreateInfo.edges;
  m_backEdges = chGraphCreateInfo.backEdges;
  m_offset = chGraphCreateInfo.offset;
  m_backOffset = chGraphCreateInfo.backOffset;
  m_numberOfNodes = chGraphCreateInfo.numberOfNodes;
  m_numberOfEdges = chGraphCreateInfo.numberOfEdges;

  m_nodesMMap = chGraphCreateInfo.nodesMMap;
  m_edgesMMap = chGraphCreateInfo.edgesMMap;
  m_backEdgesMMap = chGraphCreateInfo.backEdgesMMap;
  m_offsetMMap = chGraphCreateInfo.offsetMMap;
  m_backOffsetMMap = chGraphCreateInfo.backOffsetMMap;
}
std::shared_ptr<pathFinder::CHGraph> pathFinder::CHGraph::makeShared(pathFinder::CHGraphCreateInfo chGraphCreateInfo) {
  return std::make_shared<CHGraph>(chGraphCreateInfo);
}
void pathFinder::CHGraph::sortByLevel(std::vector<CHNode> &sortedNodes) {

  sortedNodes.reserve(m_numberOfNodes);
  for(int i = 0; i < m_numberOfNodes; ++i)
    sortedNodes.emplace_back(m_nodes[i]);
  std::sort(sortedNodes.begin(), sortedNodes.end(),
            [](const auto &node1, const auto &node2) -> bool {
              return node1.level == node2.level ? node1.id < node2.id
                                                : node1.level > node2.level;
            });
}
void pathFinder::CHGraph::randomizeLatLngs() {
  srand (static_cast <unsigned> (time(nullptr)));

  for(auto i = m_nodes; i < m_nodes + m_numberOfNodes; ++i) {
    auto LOlat = 45;
    auto HIlat = 46;
    auto lat = LOlat + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HIlat-LOlat)));
    auto LOlng = 8;
    auto HIlng = 9;
    auto lng = LOlng + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HIlng-LOlng)));
    i->latLng = LatLng{lat, lng};
  }
}
void pathFinder::CHGraph::sortEdges() {
  std::sort(m_edges, m_edges + m_numberOfEdges, [](auto edge1, auto edge2) -> bool{
    return (edge1.source == edge2.source) ? edge1.target <= edge2.target : edge1.source < edge2.source;
  });
}
pathFinder::NodeId pathFinder::CHGraph::getNodeIdFor(pathFinder::LatLng latLng) { double distance = std::numeric_limits<double>::max();
  NodeId position = m_numberOfNodes;
  for(int i = 0; i < m_numberOfNodes; ++i) {
    auto& node = m_nodes[i];
    double currentDistance = beeLineWithoutSquareRoot(node.latLng, latLng);
    if(currentDistance < distance){
      distance = currentDistance;
      position = i;
    }
  }
  std::cout << latLng.lat << latLng.lng << std::endl;
  std::cout << position << std::endl;
  return position;
}
pathFinder::CHNode pathFinder::CHGraph::getNode(pathFinder::NodeId id) const {
  return m_nodes[id];
}
std::optional<size_t> pathFinder::CHGraph::getEdgePosition(const pathFinder::CHEdge &edge,
                                                           pathFinder::EdgeDirection direction) {
  for(auto i = m_offset[edge.source]; i < m_offset[edge.source+1]; ++i) {
    auto e = m_edges[i];
    if(e.target == edge.target) {
      return i;
    }
  }
  return std::nullopt;
}
std::vector<pathFinder::CHEdge> pathFinder::CHGraph::getPathFromShortcut(pathFinder::CHEdge shortcut, double minLength) {
  std::vector<CHEdge> path;
  if(__glibc_unlikely(shortcut.source >= m_numberOfNodes || shortcut.target >= m_numberOfNodes))
    throw std::out_of_range("shortcut out of range");
  Node source = m_nodes[shortcut.source];
  Node target = m_nodes[shortcut.target];
  double length = source.euclid(target);

  if(!shortcut.child2.has_value() || length <= minLength) {
    path.push_back(shortcut);
    return path;
  }
  std::stack<uint32_t> edgesStack;

  edgesStack.push(shortcut.child2.value());
  edgesStack.push(shortcut.child1.value());

  while (!edgesStack.empty()) {
    auto& edgeIdx = edgesStack.top();
    edgesStack.pop();
    __glibcxx_assert(edgeIdx < m_numberOfEdges);
    const auto& edge = m_edges[edgeIdx];
    double length = m_nodes[edge.source].euclid(m_nodes[edge.target]);
    if(edge.child1.has_value() && length > minLength) {

      edgesStack.push(edge.child2.value());
      edgesStack.push(edge.child1.value());
    } else {
      path.push_back(edge);
    }
  }
  return path;
}
double pathFinder::CHGraph::getDistance(pathFinder::NodeId node1, pathFinder::NodeId node2) {
  return 0;
}
double pathFinder::CHGraph::beeLineWithoutSquareRoot(pathFinder::LatLng latLng1, pathFinder::LatLng latLng2) {
  return pow(latLng1.lat - latLng2.lat, 2) + pow(latLng1.lng - latLng2.lng, 2);
}
pathFinder::MyIterator<const pathFinder::CHNode *> pathFinder::CHGraph::getNodes() {
  return pathFinder::MyIterator<const pathFinder::CHNode *>(m_nodes, m_nodes + m_numberOfNodes);
}
pathFinder::MyIterator<const pathFinder::CHEdge *> pathFinder::CHGraph::getEdges() {
  return pathFinder::MyIterator<const pathFinder::CHEdge *>(m_edges, m_edges + m_numberOfEdges);
}
pathFinder::MyIterator<const pathFinder::CHEdge *> pathFinder::CHGraph::edgesFor(pathFinder::NodeId node,
                                                                                 pathFinder::EdgeDirection direction) {
  switch (direction) {
  case FORWARD:
    return {&m_edges[m_offset[node]], &m_edges[m_offset[node + 1]]};
  case BACKWARD:
    return {&m_backEdges[m_backOffset[node]], &m_backEdges[m_backOffset[node + 1]]};
  }
  return {&m_edges[m_offset[node]], &m_edges[m_offset[node + 1]]};
}
pathFinder::Level pathFinder::CHGraph::getLevel(pathFinder::NodeId nodeId) {
    return m_nodes[nodeId].level;
}
pathFinder::MyIterator<const pathFinder::NodeId *> pathFinder::CHGraph::getForwardOffset() {
  return pathFinder::MyIterator<const pathFinder::NodeId *>(m_offset, m_offset + m_numberOfNodes);
}
pathFinder::MyIterator<const pathFinder::NodeId *> pathFinder::CHGraph::getBackwardOffset() {
  return pathFinder::MyIterator<const pathFinder::NodeId *>(m_backOffset, m_backOffset + m_numberOfNodes);
}
pathFinder::CHGraph::~CHGraph() {
  pathFinder::Static::conditionalFree(m_nodes, m_nodesMMap,m_numberOfNodes * sizeof(CHNode));
  pathFinder::Static::conditionalFree(m_edges, m_edgesMMap,m_numberOfEdges * sizeof(CHEdge));
  pathFinder::Static::conditionalFree(m_backEdges, m_backEdgesMMap,m_numberOfEdges * sizeof(CHEdge));
  pathFinder::Static::conditionalFree(m_offset, m_offsetMMap,(m_numberOfNodes + 1) * sizeof(NodeId));
  pathFinder::Static::conditionalFree(m_backOffset, m_backOffsetMMap,(m_numberOfNodes + 1) * sizeof(NodeId));
}
pathFinder::CHGraph::CHGraph() {}
bool pathFinder::CHGraph::isValidNodeId(pathFinder::NodeId id) const {
  return id < m_numberOfNodes;
}
size_t pathFinder::CHGraph::getNumberOfNodes() { return m_numberOfNodes;}
pathFinder::MyIterator<const pathFinder::CHEdge *> pathFinder::CHGraph::getBackEdges() {
  return pathFinder::MyIterator<const pathFinder::CHEdge *>(m_edges, m_edges + m_numberOfEdges);
}
size_t pathFinder::CHGraph::getNumberOfEdges() { return m_numberOfEdges; }
