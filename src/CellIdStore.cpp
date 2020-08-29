//
// Created by sokol on 26.05.20.
//

#include <path_finder/helper/Static.h>
#include <path_finder/storage/CellIdStore.h>
namespace pathFinder {

CellIdStore::CellIdStore(size_t numberOfEdges) {
  _cellIds = (CellId_t*) calloc(_reservedSize, sizeof(CellId_t));
  _cellIdSize = 0;
  _offsetVector = (OffsetElement*)calloc(numberOfEdges, sizeof(OffsetElement));
  _offsetVectorSize = numberOfEdges;
}

void CellIdStore::storeCellIds(
    size_t edgeId, const std::vector<CellId_t>&cellToAdd) {
  _offsetVector[edgeId].position = _cellIdSize;
  _offsetVector[edgeId].size = cellToAdd.size();
  if(_reservedSize < _cellIdSize + cellToAdd.size()) {
    _reservedSize += cellToAdd.size();
    _reservedSize = _reservedSize * 2;
    _cellIds = (CellId_t*)realloc(_cellIds, sizeof(CellId_t) * (_reservedSize));
  }
  for(auto cell : cellToAdd) {
    _cellIds[_cellIdSize++] = cell;
  }
}

std::vector<CellId_t>
CellIdStore::getCellIds(size_t edgeId) {
  auto offsetElement = _offsetVector[edgeId];
  std::vector<CellId_t> resultVec;
  resultVec.reserve(offsetElement.size);
  for(size_t i = offsetElement.position; i <  offsetElement.position + offsetElement.size; ++i) {
    resultVec.emplace_back(_cellIds[i]);
  }
  return resultVec;
}

size_t CellIdStore::cellIdSize() const{
  return _cellIdSize;
}

size_t CellIdStore::offsetSize() const{
  return _offsetVectorSize;
}

CellIdStore::CellIdStore(CellIdStoreCreateInfo cellIdStoreCreateInfo){
  _cellIds = cellIdStoreCreateInfo.cellIds;
  _cellIdSize = cellIdStoreCreateInfo.cellIdSize;
  _offsetVector = cellIdStoreCreateInfo.offsetVector;
  _offsetVectorSize = cellIdStoreCreateInfo.offsetSize;
  _cellIdsMMap = cellIdStoreCreateInfo.cellIdsMMap;
  _offsetMMap = cellIdStoreCreateInfo.offsetMMap;
}
CellIdStore::~CellIdStore() {
  Static::conditionalFree(_cellIds, _cellIdsMMap, _cellIdSize * sizeof(CellId_t));
  Static::conditionalFree(_offsetVector, _offsetMMap, _offsetVectorSize * sizeof(CellId_t));
  Static::conditionalFree(_cellIds, _cellIdsMMap, _cellIdSize * sizeof(CellId_t));
}
void CellIdStore::shrink_to_fit() {
  _cellIds = (CellId_t*)realloc(_cellIds, sizeof(CellId_t) * (_cellIdSize));
}
}
