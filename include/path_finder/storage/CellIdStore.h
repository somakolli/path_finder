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
  CellIdStore() = delete;
  CellIdStore(CellIdStore const &) = delete;
  explicit CellIdStore(size_t numberOfEdges);
  explicit CellIdStore(CellIdStoreCreateInfo cellIdStoreCreateInfo);
  ~CellIdStore();
  void storeCellIds(size_t edgeId, const std::vector<CellId_t> &cellToAdd);
  [[nodiscard]] auto offsetSize() const -> size_t;
  [[nodiscard]] auto cellIdSize() const -> size_t;
  void shrink_to_fit();
  [[nodiscard]] auto getCellIds(size_t edgeId) const -> std::vector<CellId_t>;
  [[nodiscard]] auto getCellIds(const std::vector<size_t> &edgeId) const -> std::vector<CellId_t>;
};
} // namespace pathFinder
