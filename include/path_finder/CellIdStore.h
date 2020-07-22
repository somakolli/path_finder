//
// Created by sokol on 26.05.20.
//

#ifndef MASTER_ARBEIT_CELLIDSTORE_H
#define MASTER_ARBEIT_CELLIDSTORE_H
#include <vector>
#include <path_finder/HubLabelStore.h>

namespace pathFinder{
using CellId_t = uint32_t;
template <template <class, class> class Vector = std::vector,
    class OffsetVector = std::vector<OffsetElement>,
          class CellId_t = unsigned int>
class CellIdStore {
private:

  Vector<CellId_t, std::allocator<CellId_t>> _cellIds;
  OffsetVector _offsetVector;

public:
  explicit CellIdStore(size_t numberOfEdges);
  CellIdStore(Vector<CellId_t, std::allocator<CellId_t>> cellIds, OffsetVector offsetVector);
  void storeCellIds(size_t edgeId, const std::vector<CellId_t>&cellToAdd);
  size_t cellIdSize();
  size_t offsetSize();
  auto &cellIdsVec();
  auto &offsetVec();
  MyIterator<CellId_t*> getCellIds(size_t edgeId);
};
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
CellIdStore<Vector, OffsetVector, CellId_t>::CellIdStore(size_t numberOfEdges) {
  _offsetVector.reserve(numberOfEdges);
  for(int i = 0; i < numberOfEdges; ++i) {
    _offsetVector.push_back(OffsetElement());
  }
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
void CellIdStore<Vector, OffsetVector, CellId_t>::storeCellIds(
    size_t edgeId, const std::vector<CellId_t>&cellToAdd) {
  _offsetVector[edgeId].position = _cellIds.size();
  for(auto cell : cellToAdd) {
    _cellIds.push_back(cell);
  }
  _offsetVector[edgeId].size = cellToAdd.size();
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
MyIterator<CellId_t *>
CellIdStore<Vector, OffsetVector, CellId_t>::getCellIds(size_t edgeId) {
  auto offsetElement = _offsetVector[edgeId];
  return MyIterator<CellId_t *>(&_cellIds[offsetElement.position], &_cellIds[offsetElement.position + offsetElement.size]);
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
size_t CellIdStore<Vector, OffsetVector, CellId_t>::cellIdSize() {
  return _cellIds.size();
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
size_t CellIdStore<Vector, OffsetVector, CellId_t>::offsetSize() {
  return _offsetVector.size();
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
auto &CellIdStore<Vector, OffsetVector, CellId_t>::cellIdsVec() {
  return _cellIds;
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
auto &CellIdStore<Vector, OffsetVector, CellId_t>::offsetVec() {
  return _offsetVector;
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
CellIdStore<Vector, OffsetVector, CellId_t>::CellIdStore(
    Vector<CellId_t, std::allocator<CellId_t>> cellIds,
    OffsetVector offsetVector) : _cellIds(cellIds), _offsetVector(offsetVector) {}

}
#endif // MASTER_ARBEIT_CELLIDSTORE_H
