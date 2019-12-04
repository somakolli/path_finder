//
// Created by sokol on 02.10.19.
//

#include <cmath>
#include "../include/Graph.h"

algEng::LatLng algEng::Graph::getLatLng(algEng::NodeId nodeId) {
    auto& node = nodes[nodeId];
    return node.latLng;
}

algEng::NodeId algEng::Graph::getNodeId(algEng::LatLng latLng) {
    double smallestDistance = std::numeric_limits<double>::max();
    Node closestNode{};
    for(const auto& node : nodes) {
        double d = node.quickBeeLine(latLng);
        if(d < smallestDistance) {
            smallestDistance = d;
            closestNode = node;
        }
    }
    return closestNode.id;
}

double algEng::Node::quickBeeLine(const LatLng& other) const{
    return std::pow(other.lat - latLng.lat, 2) + std::pow(other.lng - latLng.lng, 2);
}
