//
// Created by sokol on 20.04.20.
//
#include <path_finder/graphs/CHGraph.h>
#include <path_finder/graphs/Grid.h>
namespace pathFinder {
pathFinder::Grid::Grid(uint16_t latStretchFactor, uint16_t lngStretchFactor)
    : m_latStretchFactor(latStretchFactor), m_lngStretchFactor(lngStretchFactor) {}

}
