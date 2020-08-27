//
// Created by sokol on 20.04.20.
//
#include <path_finder/graphs/CHGraph.h>
#include <path_finder/graphs/Grid.h>
namespace pathFinder {

pathFinder::Grid::Grid(double latStretchFactor, double lngStretchFactor)
    : m_latStretchFactor(latStretchFactor), m_lngStretchFactor(lngStretchFactor) {}

double Grid::getLatStretchFactor() const { return m_latStretchFactor; }
double Grid::getLngStretchFactor() const { return m_lngStretchFactor; }

Grid::GridKey Grid::getKeyFor(LatLng latLng) const{
  int latPositionInGrid = std::floor(latLng.lat * m_latStretchFactor);
  int lngPositionInGrid = std::floor(latLng.lng * m_lngStretchFactor);
  return std::make_pair(latPositionInGrid, lngPositionInGrid);
}
Grid::PositionPair Grid::operator[](LatLng latLng) const {
  if(m_grid.contains(getKeyFor(latLng))) {
    return m_grid.at(getKeyFor(latLng));
  }
  return PositionPair {0,0};
}

/*
 * the goal is to get the surrouing grid of distance range given a middlePosition
 *  o o o
 *  o 0 o
 *  o o o
 *  0 = middlePosition,  o = returned
 */
std::vector<Grid::PositionPair> Grid::getSurroundingNodes(uint32_t range, Grid::GridKey middlePosition) const {
  std::vector<PositionPair> returnVec;
  int negativeRange = -1 * int(range);

  /* top
   *  o o x
   *  x 0 x
   *  x x x
   *  x = not emplaced, 0 = middlePosition,  o = emplaced
   *
   *  y-position = negativeRange and x-position changes through the loop
  */
  for(int i = negativeRange; i < (int)range; ++i) {
    auto newGridCoords = std::make_pair(middlePosition.first + i, middlePosition.second + negativeRange);
    if(m_grid.contains(newGridCoords))
      returnVec.emplace_back(m_grid.at(newGridCoords));
  }
  /* bottom
   *  x x x
   *  x 0 x
   *  x o o
   *  x = not emplaced, 0 = middlePosition,  o = emplaced
   *
   *  y-position = range and x-position changes through the loop
  */
  for(int i = negativeRange + 1; i <= (int)range; ++i) {
    auto newGridCoords = std::make_pair(middlePosition.first + i, middlePosition.second + range);
    if(m_grid.contains(newGridCoords))
      returnVec.emplace_back(m_grid.at(newGridCoords));
  }
  /* left
   *  x x x
   *  o 0 x
   *  o x x
   *  x = not emplaced, 0 = middlePosition,  o = emplaced
   *
   *  y-position = changes through the loop and x-position = negative range
  */
  for(int i = negativeRange + 1; i <= (int)range; ++i) {
    auto newGridCoords = std::make_pair(middlePosition.first + negativeRange, middlePosition.second + i);
    if(m_grid.contains(newGridCoords))
      returnVec.emplace_back(m_grid.at(newGridCoords));
  }
  /* right
   *  x x o
   *  x 0 o
   *  x x x
   *  x = not emplaced, 0 = middlePosition,  o = emplaced
   *
   *  y-position = changes through the loop and x-position = range
  */
  for(int i = negativeRange; i < (int)range; ++i) {
    auto newGridCoords = std::make_pair(middlePosition.first + range, middlePosition.second + i);
    if(m_grid.contains(newGridCoords))
      returnVec.emplace_back(m_grid.at(newGridCoords));
  }
  return returnVec;
}
double Grid::getRadiusWithoutSqrt(uint32_t range, LatLng latLng) {
  return CHGraph::beeLineWithoutSquareRoot(latLng, LatLng(latLng.lat + (range * m_latStretchFactor), latLng.lng));
}

}
