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
  double m_latStretchFactor;
  double m_lngStretchFactor;
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
  Grid(double latStretchFactor, double lngStretchFactor);

  [[nodiscard]] double getLatStretchFactor() const;
  [[nodiscard]] double getLngStretchFactor() const;

};
} // namespace pathFinder
#endif // MASTER_ARBEIT_GRID_H
