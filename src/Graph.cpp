//
// Created by sokol on 02.10.19.
//

#include "../include/Graph.h"
#include <cmath>

pathFinder::LatLng
pathFinder::Graph::getLatLng(pathFinder::NodeId nodeId) const {
  auto &node = nodes[nodeId];
  return node.latLng;
}

pathFinder::NodeId
pathFinder::Graph::getNodeId(pathFinder::LatLng latLng) const {
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
  return std::pow(other.lat - latLng.lat, 2) +
         std::pow(other.lng - latLng.lng, 2);
}
