//
// Created by sokol on 02.12.19.
//
#include "path_finder/PathFinderBase.h"

std::vector<pathFinder::LatLng> pathFinder::PathFinderBase::getShortestPath(
    pathFinder::NodeId source, pathFinder::NodeId target, std::optional<Distance>& distance,
    std::vector<unsigned int> *cellIds) {
  return std::vector<LatLng>();
}
std::vector<pathFinder::LatLng> pathFinder::PathFinderBase::getShortestPath(
    pathFinder::LatLng source, pathFinder::LatLng target, std::optional<Distance>& distance,
    std::vector<unsigned int> *cellIds) {
  return std::vector<LatLng>();
}
