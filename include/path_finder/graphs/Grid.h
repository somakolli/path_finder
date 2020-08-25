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
private:
  std::map<std::pair<int, int>, std::pair<NodeId, NodeId>> m_grid;
  uint16_t m_latStretchFactor = 1;
  uint16_t m_lngStretchFactor = 1;
public:
  auto begin() {
      return m_grid.cbegin();
  };
  auto end() {
      return m_grid.cend();
  };
  auto &operator[](std::pair<int, int> index){
     return m_grid[index];
  };
  Grid(uint16_t latStretchFactor, uint16_t lngStretchFactor);

  uint16_t getLatStretchFactor() const;
  uint16_t getLngStretchFactor() const;
};
} // namespace pathFinder
#endif // MASTER_ARBEIT_GRID_H
