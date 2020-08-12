//
// Created by sokol on 26.05.20.
//

#ifndef MASTER_ARBEIT_CELLIDSTORE_H
#define MASTER_ARBEIT_CELLIDSTORE_H
#include <path_finder/storage/HubLabelStore.h>
#include <vector>

namespace pathFinder{
using CellId_t = uint32_t;
template <class CellId_t = unsigned int>
class CellIdStore {
private:
  size_t _cellIdSize;
  size_t _offsetVectorSize;

  CellId_t* _cellIds;
  OffsetElement* _offsetVector;

public:
  explicit CellIdStore(size_t numberOfEdges);
  CellIdStore(CellId_t* cellIds,
              size_t cellIdsSize,
              OffsetElement* offsetVector,
              size_t offsetVectorSize);
  void storeCellIds(size_t edgeId, const std::vector<CellId_t>&cellToAdd);
  size_t cellIdSize() const;
  size_t offsetSize() const;
  auto cellIdsVec();
  const auto cellIdsVec() const;
  auto offsetVec();
  const auto offsetVec() const;
  std::vector<CellId_t> getCellIds(size_t edgeId);
};
template <
    class CellId_t>
CellIdStore<CellId_t>::CellIdStore(size_t numberOfEdges) {
  _cellIds = nullptr;
  _cellIdSize = 0;
  _offsetVector = (OffsetElement*)calloc(numberOfEdges, sizeof(OffsetElement));
  _offsetVectorSize = numberOfEdges;
}
template <class CellId_t>
void CellIdStore<CellId_t>::storeCellIds(
    size_t edgeId, const std::vector<CellId_t>&cellToAdd) {
  _offsetVector[edgeId].position = _cellIdSize;
  _offsetVector[edgeId].size = cellToAdd.size();
  _cellIds = (CellId_t*)realloc(_cellIds, sizeof(CellId_t) * (_cellIdSize + cellToAdd.size()));
  for(auto cell : cellToAdd) {
    _cellIds[_cellIdSize++] = cell;
  }
}
template <class CellId_t>
std::vector<CellId_t>
CellIdStore<CellId_t>::getCellIds(size_t edgeId) {
  auto offsetElement = _offsetVector[edgeId];
  std::vector<CellId_t> resultVec;
  resultVec.reserve(offsetElement.size);
  for(size_t i = offsetElement.position; i <  offsetElement.position + offsetElement.size; ++i) {
    resultVec.emplace_back(_cellIds[i]);
  }
  return resultVec;
}
template <
    class CellId_t>
size_t CellIdStore<CellId_t>::cellIdSize() const{
  return _cellIdSize;
}
template <
    class CellId_t>
size_t CellIdStore<CellId_t>::offsetSize() const{
  return _offsetVectorSize;
}
template <class CellId_t>
auto CellIdStore<CellId_t>::cellIdsVec() {
  return _cellIds;
}
template <class CellId_t>
const auto CellIdStore<CellId_t>::cellIdsVec() const{
  return _cellIds;
}
template <class CellId_t>
auto CellIdStore<CellId_t>::offsetVec() {
  return _offsetVector;
}
template <class CellId_t>
const auto CellIdStore<CellId_t>::offsetVec() const{
  return _offsetVector;
}
template <class CellId_t>
CellIdStore<CellId_t>::CellIdStore(
    CellId_t* cellIds,
    size_t cellIdsSize,
    OffsetElement* offsetVector,
    size_t offsetVectorSize) : _cellIds(cellIds), _offsetVector(offsetVector),
                               _cellIdSize(cellIdsSize), _offsetVectorSize(offsetVectorSize) {}

}
#endif // MASTER_ARBEIT_CELLIDSTORE_H
