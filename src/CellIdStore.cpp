//
// Created by sokol on 26.05.20.
//

#include <path_finder/helper/Static.h>
#include <path_finder/storage/CellIdStore.h>
namespace pathFinder {

CellIdStore::CellIdStore(size_t numberOfEdges) : _reservedSize(numberOfEdges) {
  _cellIds = new CellId_t[_reservedSize];
  _cellIdSize = 0;
  _offsetVector = new OffsetElement[numberOfEdges];
  _offsetVectorSize = numberOfEdges;
}

CellIdStore::CellIdStore(CellIdStoreCreateInfo cellIdStoreCreateInfo) {
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
}

void CellIdStore::storeCellIds(size_t edgeId, const std::vector<CellId_t> &cellToAdd) {
  _offsetVector[edgeId].position = _cellIdSize;
  _offsetVector[edgeId].size = cellToAdd.size();
  if (_reservedSize < _cellIdSize + cellToAdd.size()) {
	auto oldSize = _reservedSize;
    _reservedSize += cellToAdd.size();
    _reservedSize = _reservedSize * 2;
	CellId_t * newCellIds = new CellId_t[_reservedSize];
	std::copy(_cellIds, _cellIds+oldSize, newCellIds);
	std::swap(newCellIds, _cellIds);
	delete[] newData;
  }
  for (auto cell : cellToAdd) {
    _cellIds[_cellIdSize++] = cell;
  }
}

auto CellIdStore::getCellIds(size_t edgeId) const -> std::vector<CellId_t> {
  auto offsetElement = _offsetVector[edgeId];
  std::vector<CellId_t> resultVec;
  resultVec.reserve(offsetElement.size);
  for (size_t i = offsetElement.position; i < offsetElement.position + offsetElement.size; ++i) {
    resultVec.emplace_back(_cellIds[i]);
  }
  return resultVec;
}

auto CellIdStore::cellIdSize() const -> size_t { return _cellIdSize; }

auto CellIdStore::offsetSize() const -> size_t { return _offsetVectorSize; }

void CellIdStore::shrink_to_fit() {
	CellId_t * newData = new CellId_t[_cellIdSize];
	std::copy(_cellIds, _cellIds+_cellIdSize, newData);
	std::swap(newData, _cellIds);
	delete[] newData;
	_reservedSize = _cellIdSize;
}
auto CellIdStore::getCellIds(const std::vector<size_t> &edgeIds) const -> std::vector<CellId_t> {
  std::vector<CellId_t> cellIds;
  for (auto edgeId : edgeIds) {
    auto currentCellIds = getCellIds(edgeId);
    cellIds.insert(cellIds.end(), currentCellIds.cbegin(), currentCellIds.cend());
  }
  return cellIds;
}
} // namespace pathFinder
