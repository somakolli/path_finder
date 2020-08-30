#pragma once
#include <path_finder/helper/Types.h>

#include <vector>

namespace pathFinder {

struct CellIdStoreCreateInfo {
  CellId_t *cellIds = nullptr;
  OffsetElement *offsetVector = nullptr;
  size_t cellIdSize = 0;
  size_t offsetSize = 0;
  bool cellIdsMMap = false;
  bool offsetMMap = false;
};
class CellIdStore {
  friend class FileWriter;

private:
  size_t _cellIdSize;
  size_t _offsetVectorSize;
  size_t _reservedSize = 100;

  CellId_t *_cellIds;
  OffsetElement *_offsetVector;

  bool _cellIdsMMap = true;
  bool _offsetMMap = true;

public:
  explicit CellIdStore(size_t numberOfEdges);
  explicit CellIdStore(CellIdStoreCreateInfo cellIdStoreCreateInfo);
  ~CellIdStore();
  void storeCellIds(size_t edgeId, const std::vector<CellId_t> &cellToAdd);
  [[nodiscard]] size_t offsetSize() const;
  [[nodiscard]] size_t cellIdSize() const;
  void shrink_to_fit();
  [[nodiscard]] std::vector<CellId_t> getCellIds(size_t edgeId) const;
  [[nodiscard]] std::vector<CellId_t> getCellIds(const std::vector<size_t> &edgeId) const;
};
} // namespace pathFinder
