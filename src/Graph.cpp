//
// Created by sokol on 02.10.19.
//

#include "path_finder/graphs/Graph.h"
#include <cmath>

[[maybe_unused]] pathFinder::LatLng pathFinder::Graph::getLatLng(pathFinder::NodeId nodeId) const {
  auto &node = nodes[nodeId];
  return node.latLng;
}

[[maybe_unused]] pathFinder::NodeId pathFinder::Graph::getNodeId(pathFinder::LatLng latLng) const {
  double smallestDistance = std::numeric_limits<double>::max();
  Node closestNode{};
  for (const auto &node : nodes) {
    double d = node.quickBeeLine(latLng);
    if (d < smallestDistance) {
      smallestDistance = d;
      closestNode = node;
    }
  }
  return closestNode.id;
}

double pathFinder::Node::quickBeeLine(const LatLng &other) const {
  return std::pow(other.lat - latLng.lat, 2) + std::pow(other.lng - latLng.lng, 2);
}

double pathFinder::Node::quickBeeLine(const Node &other) const { return quickBeeLine(other.latLng); }
double pathFinder::Node::euclid(const pathFinder::Node &other) const { return std::sqrt(quickBeeLine(other)); }
