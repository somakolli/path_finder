//
// Created by sokol on 02.12.19.
//
#include "../include/PathFinderBase.h"

std::optional<pathFinder::Distance>
pathFinder::PathFinderBase::getShortestDistance(pathFinder::NodeId source, pathFinder::NodeId target) {
    return std::optional<Distance>();
}

std::vector<pathFinder::LatLng> pathFinder::PathFinderBase::getShortestPath(pathFinder::NodeId source, pathFinder::NodeId target) {
    return std::vector<LatLng>();
}
