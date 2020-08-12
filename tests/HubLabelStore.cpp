//
// Created by sokol on 23.01.20.
//
#include "path_finder/storage/HubLabelStore.h"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <path_finder/storage/FileLoader.h>
#include <path_finder/storage/FileWriter.h>
#include <vector>

TEST(HubLabelStore, ConstructstCorrectly) {
    pathFinder::HubLabelStore hubLabelStore(0);
    ASSERT_EQ(0, hubLabelStore.getNumberOfLabels());
}

TEST(HubLabelStore, StoreAndRetrieveTest) {
    pathFinder::HubLabelStore hubLabelStore(2);
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
    hubLabelStore.store(vec2, 1, pathFinder::EdgeDirection::BACKWARD);
    auto resultVec1 = hubLabelStore.retrieve(0, pathFinder::EdgeDirection::FORWARD);
    auto resultVec2 = hubLabelStore.retrieve(1, pathFinder::EdgeDirection::BACKWARD);
    int i = 0;
    for(pathFinder::CostNode costNode : resultVec1) {
      ASSERT_EQ(vec1[i++],costNode);
    }
    i = 0;
    for(pathFinder::CostNode costNode : resultVec2) {
      ASSERT_EQ(vec2[i++],costNode);
    }
}

TEST(HubLabelStore, StoreAndRetrieveMmap) {
    auto hubLabelStore = pathFinder::HubLabelStore(2);
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
    hubLabelStore.store(vec2, 1, pathFinder::EdgeDirection::BACKWARD);

    pathFinder::FileWriter::writeHubLabels(hubLabelStore, "testGraph", "testGraph/");
    auto mmapHubLabelStore = pathFinder::FileLoader::loadHubLabels("testGraph");

    auto resultVec1 = mmapHubLabelStore->retrieve(0, pathFinder::EdgeDirection::FORWARD);
    auto resultVec2 = mmapHubLabelStore->retrieve(1, pathFinder::EdgeDirection::BACKWARD);
    int i = 0;
    for(pathFinder::CostNode costNode : resultVec1) {
      ASSERT_EQ(vec1[i++],costNode);
    }
    i = 0;
    for(pathFinder::CostNode costNode : resultVec2) {
      ASSERT_EQ(vec2[i++],costNode);
    }
}

/*TEST(HubLabelStore, StoreMMap) {
  auto hubLabelStore = pathFinder::HubLabelStore(2, true);
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
  hubLabelStore.store(vec2, 1, pathFinder::EdgeDirection::BACKWARD);
}*/