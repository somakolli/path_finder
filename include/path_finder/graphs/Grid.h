//
// Created by sokol on 20.04.20.
//

#ifndef MASTER_ARBEIT_GRID_H
#define MASTER_ARBEIT_GRID_H
#include <cmath>
#include <map>
#include <utility>
namespace pathFinder {
class Grid {
public:
  using PositionPair = std::pair<NodeId, NodeId>;
  using GridKey = std::pair<int, int>;
private:
  std::map<GridKey, PositionPair> m_grid;
  uint16_t m_latStretchFactor;
  uint16_t m_lngStretchFactor;
public:
  auto begin() {
      return m_grid.cbegin();
  };
  auto end() {
      return m_grid.cend();
  };
  auto &operator[](GridKey index){
     return m_grid[index];
  };
  [[nodiscard]] GridKey getKeyFor(LatLng latLng) const;
  PositionPair operator[](LatLng latLng) const;
  Grid(uint16_t latStretchFactor, uint16_t lngStretchFactor);

  [[nodiscard]] uint16_t getLatStretchFactor() const;
  [[nodiscard]] uint16_t getLngStretchFactor() const;

};
} // namespace pathFinder
#endif // MASTER_ARBEIT_GRID_H
