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

    std::vector<pathFinder::CostNode> resultVec1 = hubLabelStore.retrieve(0, pathFinder::EdgeDirection::FORWARD);

  std::vector<pathFinder::CostNode> resultVec2 = hubLabelStore.retrieve(1, pathFinder::EdgeDirection::BACKWARD);
    int i = 0;
    for(pathFinder::CostNode costNode : resultVec1) {
      ASSERT_EQ(vec1[i++],costNode);
    }
    i = 0;
    for(pathFinder::CostNode costNode : resultVec2) {
      ASSERT_EQ(vec2[i++],costNode);
    }
}

TEST(HubLabelStore, StoreAndRetrieveMulti) {
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
    std::vector<std::pair<pathFinder::NodeId, pathFinder::costNodeVec_t>> idLabelVec;
    idLabelVec.emplace_back(0, vec1);
    idLabelVec.emplace_back(1, vec2);
    hubLabelStore.store(idLabelVec, pathFinder::EdgeDirection::FORWARD);
    hubLabelStore.store(idLabelVec, pathFinder::EdgeDirection::BACKWARD);
  {
    std::vector<pathFinder::CostNode> resultVec1 = hubLabelStore.retrieve(0, pathFinder::EdgeDirection::FORWARD);
    std::vector<pathFinder::CostNode> resultVec2 = hubLabelStore.retrieve(1, pathFinder::EdgeDirection::FORWARD);

    ASSERT_EQ(resultVec1.size(), vec1.size());
    ASSERT_EQ(resultVec2.size(), vec2.size());

    int i = 0;

    for(pathFinder::CostNode costNode : resultVec1) {
      ASSERT_EQ(vec1[i++],costNode);
    }
    i = 0;
    for(pathFinder::CostNode costNode : resultVec2) {
      ASSERT_EQ(vec2[i++],costNode);
    }
  }
  {
    std::vector<pathFinder::CostNode> resultVec1 = hubLabelStore.retrieve(0, pathFinder::EdgeDirection::BACKWARD);
    std::vector<pathFinder::CostNode> resultVec2 = hubLabelStore.retrieve(1, pathFinder::EdgeDirection::BACKWARD);

    ASSERT_EQ(resultVec1.size(), vec1.size());
    ASSERT_EQ(resultVec2.size(), vec2.size());

    int i = 0;
    for(pathFinder::CostNode costNode : resultVec1) {
      ASSERT_EQ(vec1[i++],costNode);
    }
    i = 0;
    for(pathFinder::CostNode costNode : resultVec2) {
      ASSERT_EQ(vec2[i++],costNode);
    }
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

    pathFinder::costNodeVec_t resultVec1 = mmapHubLabelStore->retrieve(0, pathFinder::EdgeDirection::FORWARD);

  pathFinder::costNodeVec_t resultVec2 = mmapHubLabelStore->retrieve(1, pathFinder::EdgeDirection::BACKWARD);
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