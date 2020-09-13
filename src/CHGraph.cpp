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
  midPoint = chGraphCreateInfo.midPoint;
  boundingBox = chGraphCreateInfo.boundingBox;
  grid = chGraphCreateInfo.grid;
}
[[maybe_unused]] std::shared_ptr<pathFinder::CHGraph>
pathFinder::CHGraph::makeShared(const pathFinder::CHGraphCreateInfo &chGraphCreateInfo) {
  return std::make_shared<CHGraph>(chGraphCreateInfo);
}
void pathFinder::CHGraph::sortByLevel(std::vector<CHNode> &sortedNodes) {

  sortedNodes.reserve(m_numberOfNodes);
  for (int i = 0; i < m_numberOfNodes; ++i)
    sortedNodes.emplace_back(m_nodes[i]);
  std::sort(sortedNodes.begin(), sortedNodes.end(), [](const auto &node1, const auto &node2) -> bool {
    return node1.level == node2.level ? node1.id < node2.id : node1.level > node2.level;
  });
}
void pathFinder::CHGraph::randomizeLatLngs() {
  srand(static_cast<unsigned>(time(nullptr)));

  for (auto i = m_nodes; i < m_nodes + m_numberOfNodes; ++i) {
    auto LOlat = 45;
    auto HIlat = 46;
    auto lat = LOlat + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HIlat - LOlat)));
    auto LOlng = 8;
    auto HIlng = 9;
    auto lng = LOlng + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (HIlng - LOlng)));
    i->latLng = LatLng{lat, lng};
  }
}
void pathFinder::CHGraph::sortEdges() {
  std::vector<std::pair<CHEdge, size_t>> indexEdges{};
  indexEdges.resize(m_numberOfEdges);
  int i = 0;
  std::transform(m_edges, m_edges + m_numberOfEdges, indexEdges.begin(),
                 [&i](auto &&edge) { return std::make_pair(edge, i++); });
  std::sort(indexEdges.begin(), indexEdges.end(), [](auto edge1, auto edge2) -> bool {
    return (edge1.first.source == edge2.first.source) ? edge1.first.target <= edge2.first.target
                                                      : edge1.first.source < edge2.first.source;
  });
  std::map<NodeId, NodeId> previousIdToId;
  for (int j = 0; j < indexEdges.size(); ++j) {
    previousIdToId[indexEdges[j].second] = j;
  }
  int j = 0;
  for (auto &indexEdge : indexEdges) {
    auto &edge = indexEdge.first;
    if (edge.child2.has_value())
      edge.child2 = std::optional(previousIdToId[edge.child2.value()]);
    if (edge.child1.has_value())
      edge.child1 = std::optional(previousIdToId[edge.child1.value()]);
    m_edges[j++] = edge;
  }
}
pathFinder::NodeId pathFinder::CHGraph::getNodeIdFor(pathFinder::LatLng latLng, pathFinder::LatLng other ) const {
  double distance = std::numeric_limits<double>::max();
  NodeId position = m_numberOfNodes;
  if (!boundingBox.contains(latLng) && boundingBox.contains(other)) {
    // get point at correct edge of bounding box
    latLng = boundingBox.getIntersectionPoint(other, latLng);
  } else if(!boundingBox.contains(latLng) && !boundingBox.contains(other)) {
    latLng = boundingBox.getIntersectionPoint(midPoint, latLng);
  }
  auto gridPositions = (*grid)[latLng];
  uint32_t maxSearchRadius = 10000000;
  for (int i = gridPositions.first; i < gridPositions.second; ++i) {
    auto newDistance = beeLineWithoutSquareRoot(m_nodes[i].latLng, latLng);
    if (newDistance < distance) {
      distance = newDistance;
      position = i;
    }
  }
  for (int i = 1;
       position == m_numberOfNodes && i < maxSearchRadius && distance > grid->getRadiusWithoutSqrt(i - 1, latLng);
       ++i) {
    auto firstSurround = grid->getSurroundingNodes(i, grid->getKeyFor(latLng));
    for (auto [begin, end] : firstSurround) {
      for (int j = begin; j < end; ++j) {
        auto newDistance = beeLineWithoutSquareRoot(m_nodes[i].latLng, latLng);
        if (newDistance < distance) {
          distance = newDistance;
          position = j;
        }
      }
    }
  }
  return position;
}
auto pathFinder::CHGraph::getNode(pathFinder::NodeId id) const -> pathFinder::CHNode { return m_nodes[id]; }
auto pathFinder::CHGraph::getEdgePosition(const pathFinder::CHEdge &edge,
                                                           pathFinder::EdgeDirection direction) const -> std::optional<size_t> {
  return getEdgePosition(edge.source, edge.target, direction);
}
auto pathFinder::CHGraph::getPathFromShortcut(pathFinder::CHEdge shortcut,
                                              double minLength) const -> std::vector<pathFinder::CHEdge> {
  std::vector<CHEdge> path;
  if (__glibc_unlikely(shortcut.source >= m_numberOfNodes || shortcut.target >= m_numberOfNodes))
    throw std::out_of_range("shortcut out of range");

  if (!shortcut.child2.has_value()) {
    path.push_back(shortcut);
    return path;
  }
  std::stack<uint32_t> edgesStack;

  edgesStack.push(shortcut.child2.value());
  edgesStack.push(shortcut.child1.value());

  while (!edgesStack.empty()) {
    auto &edgeIdx = edgesStack.top();
    edgesStack.pop();
    const auto &edge = m_edges[edgeIdx];
    if (edge.child1.has_value()) {
      edgesStack.push(edge.child2.value());
      edgesStack.push(edge.child1.value());
    } else {
      path.push_back(edge);
    }
  }
  return path;
}
auto pathFinder::CHGraph::beeLineWithoutSquareRoot(pathFinder::LatLng latLng1, pathFinder::LatLng latLng2) -> double {
  return pow(latLng1.lat - latLng2.lat, 2) + pow(latLng1.lng - latLng2.lng, 2);
}
auto pathFinder::CHGraph::getNodes() const -> pathFinder::MyIterator<const pathFinder::CHNode *> {
  return pathFinder::MyIterator<const pathFinder::CHNode *>(m_nodes, m_nodes + m_numberOfNodes);
}
auto pathFinder::CHGraph::getEdges() const -> pathFinder::MyIterator<const pathFinder::CHEdge *> {
  return pathFinder::MyIterator<const pathFinder::CHEdge *>(m_edges, m_edges + m_numberOfEdges);
}
auto
pathFinder::CHGraph::edgesFor(pathFinder::NodeId node, pathFinder::EdgeDirection direction) const ->
    pathFinder::MyIterator<const pathFinder::CHEdge *>
{
  switch (direction) {
  case FORWARD:
    return {&m_edges[m_offset[node]], &m_edges[m_offset[node + 1]]};
  case BACKWARD:
    return {&m_backEdges[m_backOffset[node]], &m_backEdges[m_backOffset[node + 1]]};
  }
  return {&m_edges[m_offset[node]], &m_edges[m_offset[node + 1]]};
}
auto pathFinder::CHGraph::getLevel(pathFinder::NodeId nodeId) const -> pathFinder::Level { return m_nodes[nodeId].level; }
auto pathFinder::CHGraph::getForwardOffset() const -> pathFinder::MyIterator<const pathFinder::NodeId *> {
  return pathFinder::MyIterator<const pathFinder::NodeId *>(m_offset, m_offset + m_numberOfNodes);
}
auto pathFinder::CHGraph::getBackwardOffset() const -> pathFinder::MyIterator<const pathFinder::NodeId *> {
  return pathFinder::MyIterator<const pathFinder::NodeId *>(m_backOffset, m_backOffset + m_numberOfNodes);
}
pathFinder::CHGraph::~CHGraph() {
  pathFinder::Static::conditionalFree(m_nodes, m_nodesMMap, m_numberOfNodes * sizeof(CHNode));
  pathFinder::Static::conditionalFree(m_edges, m_edgesMMap, m_numberOfEdges * sizeof(CHEdge));
  pathFinder::Static::conditionalFree(m_backEdges, m_backEdgesMMap, m_numberOfEdges * sizeof(CHEdge));
  pathFinder::Static::conditionalFree(m_offset, m_offsetMMap, (m_numberOfNodes + 1) * sizeof(NodeId));
  pathFinder::Static::conditionalFree(m_backOffset, m_backOffsetMMap, (m_numberOfNodes + 1) * sizeof(NodeId));
}
pathFinder::CHGraph::CHGraph() {}
auto pathFinder::CHGraph::isValidNodeId(pathFinder::NodeId id) const -> bool { return id < m_numberOfNodes; }
auto pathFinder::CHGraph::getNumberOfNodes() const -> size_t { return m_numberOfNodes; }
pathFinder::MyIterator<const pathFinder::CHEdge *> pathFinder::CHGraph::getBackEdges() const {
  return pathFinder::MyIterator<const pathFinder::CHEdge *>(m_edges, m_edges + m_numberOfEdges);
}
auto pathFinder::CHGraph::getNumberOfEdges() const -> size_t { return m_numberOfEdges; }
auto pathFinder::CHGraph::getNodesMutable() -> pathFinder::MyIterator<pathFinder::CHNode *> {
  return MyIterator<pathFinder::CHNode *>(m_nodes, m_nodes + m_numberOfNodes);
}
auto pathFinder::CHGraph::getEdgesMutable() -> pathFinder::MyIterator<pathFinder::CHEdge *> {
  return pathFinder::MyIterator<pathFinder::CHEdge *>(m_edges, m_edges + m_numberOfEdges);
}
auto pathFinder::CHGraph::getBackEdgesMutable() -> pathFinder::MyIterator<pathFinder::CHEdge *> {
  return pathFinder::MyIterator<pathFinder::CHEdge *>(m_backEdges, m_backEdges + m_numberOfEdges);
}
auto pathFinder::CHGraph::getEdgePosition(pathFinder::NodeId source, pathFinder::NodeId target,
                                          pathFinder::EdgeDirection direction) const -> std::optional<size_t> {
  auto offsetPointer = direction ? m_offset : m_backOffset;
  auto edgePointer = direction ? m_edges : m_backEdges;
  for (auto i = offsetPointer[source]; i < offsetPointer[source + 1]; ++i) {
    auto e = edgePointer[i];
    if (e.target == target) {
      return i;
    }
  }
  return std::nullopt;
}
auto pathFinder::CHGraph::getDepthFirstSearchOrdering(size_t startEdgeId) const -> std::vector<size_t> {
  std::vector<size_t> orderingVec;
  orderingVec.reserve(m_numberOfEdges);
  std::vector<bool> discovered;
  discovered.resize(m_numberOfEdges, false);
  std::stack<std::pair<NodeId, NodeId>> stack;
  auto startEdge = m_edges[startEdgeId];
  stack.emplace(startEdge.source, startEdge.target);
  while (!stack.empty()) {
    auto [source, target] = stack.top();
    stack.pop();
    auto positionOpt = getEdgePosition(source, target, EdgeDirection::FORWARD);
    if(!positionOpt.has_value())
      continue;
    auto edgeIdx = positionOpt.value();
    orderingVec.emplace_back(edgeIdx);
    discovered[edgeIdx] = true;
    for(auto edge: edgesFor(target, EdgeDirection::FORWARD)) {
      auto edgeId = getEdgePosition(edge, EdgeDirection::FORWARD);
      if(edgeId.has_value() && !discovered[edgeId.value()])
        stack.emplace(edge.source, edge.target);
    }
  }
  for(int i = 0; i < discovered.size(); ++i) {
    if(!discovered[i])
      orderingVec.emplace_back(i);
  }
  return orderingVec;
}
