//
// Created by sokol on 23.01.20.
//
#include <gtest/gtest.h>
#include "../include/HubLabelStore.h"
#include "../include/MmapVector.h"
#include <vector>
#include <gmock/gmock-matchers.h>

TEST(HubLabelStore, ConstructstCorrectly) {
    pathFinder::HubLabelStore<std::vector, int, std::allocator<int>> hubLabelStore(0);
    ASSERT_EQ(0, hubLabelStore.numberOfLabels());
}

TEST(HubLabelStore, StoreAndRetrieveTest) {
    pathFinder::HubLabelStore<std::vector, int, std::allocator<int>> hubLabelStore(2);
    std::vector<int> vec1 = {0, 1, 2};
    std::vector<int> vec2 = {3, 4, 5};
    hubLabelStore.store(vec1, 0, pathFinder::EdgeDirection::FORWARD);
    hubLabelStore.store(vec2, 1, pathFinder::EdgeDirection::FORWARD);
    auto resultVec1 = hubLabelStore.retrieve(0, pathFinder::EdgeDirection::FORWARD);
    auto resultVec2 = hubLabelStore.retrieve(1, pathFinder::EdgeDirection::FORWARD);
    ASSERT_THAT(vec1, testing::ElementsAre(0, 1, 2));
    ASSERT_THAT(vec2, testing::ElementsAre(3, 4, 5));
}
TEST(HubLabelStore, StoreAndRetrieveMmap) {
    pathFinder::HubLabelStore<std::vector, int, std::allocator<int>> hubLabelStore(2);
    std::vector<int> vec1 = {0, 1, 2};
    std::vector<int> vec2 = {3, 4, 5};
    hubLabelStore.store(vec1, 0, pathFinder::EdgeDirection::FORWARD);
    hubLabelStore.store(vec2, 1, pathFinder::EdgeDirection::FORWARD);
    pathFinder::HubLabelStore mmapHubLabelStore(hubLabelStore.getForwardLabels(), hubLabelStore.getBackwardLabels(), hubLabelStore.getForwardOffset(), hubLabelStore.getBackwardOffset());
    auto resultVec1 = mmapHubLabelStore.retrieve(0, pathFinder::EdgeDirection::FORWARD);
    auto resultVec2 = mmapHubLabelStore.retrieve(1, pathFinder::EdgeDirection::FORWARD);
    ASSERT_THAT(vec1, testing::ElementsAre(0, 1, 2));
    ASSERT_THAT(vec2, testing::ElementsAre(3, 4, 5));
}
