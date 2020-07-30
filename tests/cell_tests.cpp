#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <path_finder/CellIdStore.h>
#include <path_finder/Static.h>
#include <path_finder/Types.h>

namespace pathFinder{
TEST(CellIdStore, StoreAndRetrieveWorks) {
  CellIdStore store(10);
  std::vector<uint32_t> cellIds0 = {
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10
  };
  std::vector<uint32_t> cellIds1 = {2};
  std::vector<uint32_t> cellIds2 = {
      1, 2, 3, 4, 5
  };
  store.storeCellIds(0, cellIds0);
  store.storeCellIds(1, cellIds1);
  store.storeCellIds(2, cellIds2);
  {
    int i = 0;
    for (auto cellId : store.getCellIds(0)) {
      ASSERT_EQ(cellId, cellIds0[i]);
      ++i;
    }
    ASSERT_EQ(store.getCellIds(1).size(), 1);
    int j = 0;
    for (auto cellId : store.getCellIds(2)) {
      ASSERT_EQ(cellId, cellIds2[j]);
      ++j;
    }
  }

  BinaryFileDescription ids{};
  ids.size = store.cellIdsVec().size();
  ids.path = "ids";

  BinaryFileDescription offset{};
  offset.size = store.offsetSize();
  offset.path = "offset";

  Static::writeVectorToFile(store.cellIdsVec(), (ids.path).c_str());
  Static::writeVectorToFile(store.offsetVec(), (offset.path).c_str());

  auto cellIds = Static::getFromFileMMap<CellId_t>(ids, "");
  auto cellIdsOffset = Static::getFromFileMMap<OffsetElement>(offset, "");

  auto mmapStore = std::make_shared<MMapCellIdStore>(CellIdStore(cellIds, cellIdsOffset));
  {
    int i = 0;
    for (auto cellId : mmapStore->getCellIds(0)) {
      ASSERT_EQ(cellId, cellIds0[i]);
      ++i;
    }
    ASSERT_EQ(mmapStore->getCellIds(1).size(), 1);
    int j = 0;
    for (auto cellId : mmapStore->getCellIds(2)) {
      ASSERT_EQ(cellId, cellIds2[j]);
      ++j;
    }
  }
}
}

