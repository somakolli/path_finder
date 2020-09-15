#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <path_finder/helper/Static.h>
#include <path_finder/helper/Types.h>
#include <path_finder/storage/CellIdStore.h>
#include <path_finder/storage/FileLoader.h>
#include <path_finder/storage/FileWriter.h>
#include <stdio.h>

namespace pathFinder {
TEST(CellIdStore, StoreAndRetrieveWorks) {
  CellIdStore store(10);
  std::vector<uint32_t> cellIds0 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::vector<uint32_t> cellIds1 = {};
  std::vector<uint32_t> cellIds2 = {1, 2, 3, 4, 5};
  store.storeCellIds(0, cellIds0);
  store.storeCellIds(1, cellIds1);
  store.storeCellIds(2, cellIds2);
  {
    int i = 0;
    for (auto cellId : store.getCellIds(0)) {
      ASSERT_EQ(cellId, cellIds0[i]);
      ++i;
    }
    ASSERT_EQ(store.getCellIds(1).size(), 0);
    int j = 0;
    for (auto cellId : store.getCellIds(2)) {
      ASSERT_EQ(cellId, cellIds2[j]);
      ++j;
    }
  }

}
} // namespace pathFinder
