//
// Created by sokol on 26.05.20.
//

#ifndef MASTER_ARBEIT_CELLIDSTORE_H
#define MASTER_ARBEIT_CELLIDSTORE_H
#include <vector>
#include <path_finder/HubLabelStore.h>

namespace pathFinder{
template <template <class, class> class Vector = std::vector,
    class OffsetVector = std::vector<OffsetElement>,
          class CellId_t = unsigned int>
class CellIdStore {
private:

  Vector<CellId_t, std::allocator<CellId_t>> cellIds;
  OffsetVector offsetVector;

public:
  explicit CellIdStore(size_t numberOfEdges);
  void storeCellIds(size_t edgeId, const std::vector<CellId_t>&cellToAdd);
  MyIterator<CellId_t*> getCellIds(size_t edgeId);
};
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
CellIdStore<Vector, OffsetVector, CellId_t>::CellIdStore(size_t numberOfEdges) {
  offsetVector.reserve(numberOfEdges);
  for(int i = 0; i < numberOfEdges; ++i) {
    offsetVector.emplace_back(numberOfEdges);
  }
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
void CellIdStore<Vector, OffsetVector, CellId_t>::storeCellIds(
    size_t edgeId, const std::vector<CellId_t>&cellToAdd) {
  for(auto cell : cellToAdd) {
    cellIds.push_back(cell);
  }
  offsetVector[edgeId].position = cellIds.size();
  offsetVector[edgeId].size = cellToAdd.size();
}
template <template <class, class> class Vector,
    class OffsetVector,
    class CellId_t>
MyIterator<CellId_t *>
CellIdStore<Vector, OffsetVector, CellId_t>::getCellIds(size_t edgeId) {
  auto offsetElement = offsetVector[edgeId];
  return MyIterator<CellId_t *>(&cellIds[offsetElement.position], &cellIds[offsetElement.position + offsetElement.size]);
}

}
#endif // MASTER_ARBEIT_CELLIDSTORE_H
