//
// Created by sokol on 02.10.19.
//

#include <cmath>
#include "../include/Graph.h"

template<typename edgeVector, typename nodeVector, typename offsetVector>
offsetVector& pathFinder::Graph<edgeVector, nodeVector, offsetVector>::offset() {
    return _offset;
}

template<typename edgeVector, typename nodeVector, typename offsetVector>
edgeVector& pathFinder::Graph<edgeVector, nodeVector, offsetVector>::edges() {
    return _edges;
}

template<typename edgeVector, typename nodeVector, typename offsetVector>
nodeVector& pathFinder::Graph<edgeVector, nodeVector, offsetVector>::nodes() {
    return _nodes;
}

template<typename edgeVector, typename nodeVector, typename offsetVector>
pathFinder::LatLng pathFinder::Graph<edgeVector, nodeVector, offsetVector>::getLatLng(pathFinder::NodeId nodeId) const {
    auto& node = nodes()[nodeId];
    return LatLng{node.lat, node.lng};
}

template<typename edgeVector, typename nodeVector, typename offsetVector>
pathFinder::NodeId pathFinder::Graph<edgeVector, nodeVector, offsetVector>::getNodeId(pathFinder::LatLng latLng) const {
    double smallestDistance = std::numeric_limits<double>::max();
    Node closestNode{};
    for(const auto& node : nodes()) {
        double d = node.quickBeeLine(latLng);
        if(d < smallestDistance) {
            smallestDistance = d;
            closestNode = node;
        }
    }
    return closestNode.id;
}

double pathFinder::Node::quickBeeLine(const LatLng& other) const{
    return std::pow(other.lat - lat, 2) + std::pow(other.lng - lng, 2);
}
