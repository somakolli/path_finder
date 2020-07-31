//
// Created by sokol on 23.01.20.
//
#include "path_finder/storage/HubLabelStore.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <vector>

TEST(HubLabelStore, ConstructstCorrectly) {
    pathFinder::HubLabelStore<std::vector> hubLabelStore(0);
    ASSERT_EQ(0, hubLabelStore.numberOfLabels());
}

TEST(HubLabelStore, StoreAndRetrieveTest) {
    pathFinder::HubLabelStore<std::vector> hubLabelStore(2);
    std::vector<pathFinder::CostNode> vec1 = {
            pathFinder::CostNode{0, 1, 1},
            pathFinder::CostNode{1, 1, 1},
            pathFinder::CostNode{2, 1, 1}
    };
    std::vector<pathFinder::CostNode> vec2 = {
            pathFinder::CostNode{3, 1, 1},
            pathFinder::CostNode{4, 1, 1},
            pathFinder::CostNode{5, 1, 1}
    };
    hubLabelStore.store(vec1, 0, pathFinder::EdgeDirection::FORWARD);
    hubLabelStore.store(vec2, 1, pathFinder::EdgeDirection::FORWARD);
    auto resultVec1 = hubLabelStore.retrieve(0, pathFinder::EdgeDirection::FORWARD);
    auto resultVec2 = hubLabelStore.retrieve(1, pathFinder::EdgeDirection::FORWARD);
    ASSERT_THAT(vec1, testing::ElementsAre(
            pathFinder::CostNode{0, 1, 1},
            pathFinder::CostNode{1, 1, 1},
            pathFinder::CostNode{2, 1, 1})
            );
    ASSERT_THAT(vec2, testing::ElementsAre(
            pathFinder::CostNode{3, 1, 1},
            pathFinder::CostNode{4, 1, 1},
            pathFinder::CostNode{5, 1, 1})
            );
}
TEST(HubLabelStore, StoreAndRetrieveMmap) {
    pathFinder::HubLabelStore<std::vector> hubLabelStore(2);
    std::vector<pathFinder::CostNode> vec1 = {
            pathFinder::CostNode{0, 1, 1},
            pathFinder::CostNode{1, 1, 1},
            pathFinder::CostNode{2, 1, 1}
    };
    std::vector<pathFinder::CostNode> vec2 = {
            pathFinder::CostNode{3, 1, 1},
            pathFinder::CostNode{4, 1, 1},
            pathFinder::CostNode{5, 1, 1}
    };
    hubLabelStore.store(vec1, 0, pathFinder::EdgeDirection::FORWARD);
    hubLabelStore.store(vec2, 1, pathFinder::EdgeDirection::FORWARD);
    pathFinder::HubLabelStore mmapHubLabelStore(hubLabelStore.getForwardLabels(), hubLabelStore.getBackwardLabels(), hubLabelStore.getForwardOffset(), hubLabelStore.getBackwardOffset());
    auto resultVec1 = mmapHubLabelStore.retrieve(0, pathFinder::EdgeDirection::FORWARD);
    auto resultVec2 = mmapHubLabelStore.retrieve(1, pathFinder::EdgeDirection::FORWARD);
    ASSERT_THAT(vec1, testing::ElementsAre(
            pathFinder::CostNode{0, 1, 1},
            pathFinder::CostNode{1, 1, 1},
            pathFinder::CostNode{2, 1, 1})
    );
    ASSERT_THAT(vec2, testing::ElementsAre(
            pathFinder::CostNode{3, 1, 1},
            pathFinder::CostNode{4, 1, 1},
            pathFinder::CostNode{5, 1, 1})
    );
}
