//
// Created by sokol on 27.01.20.
//
#include <gtest/gtest.h>
#include "../include/HubLabelStore.h"
#include "../include/HubLabels.h"
#include <vector>
#include <gmock/gmock-matchers.h>
TEST(HubLabels, MergeWorks){
    std::vector<pathFinder::CostNode> costNodeVec1 = {
            pathFinder::CostNode{0, 0},
            pathFinder::CostNode{2, 2},
            pathFinder::CostNode{4, 4},
            pathFinder::CostNode{6, 6}
    };
    std::vector<pathFinder::CostNode> costNodeVec2 = {
            pathFinder::CostNode(1, 0),
            pathFinder::CostNode(3, 2),
            pathFinder::CostNode(5, 4),
            pathFinder::CostNode(6, 5),
            pathFinder::CostNode(7, 6),
    };
    std::vector<pathFinder::CostNode> resultVec = pathFinder::HubLabels<pathFinder::HubLabelStore<std::vector, pathFinder::CostNode, std::allocator<pathFinder::CostNode>>,
            pathFinder::CHGraph<std::vector<pathFinder::CHNode>, std::vector<pathFinder::Edge>, std::vector<pathFinder::NodeId>>>::mergeLabels(costNodeVec1, pathFinder::MyIterator(costNodeVec2.begin().base(), costNodeVec2.begin().base()), 1);
    for(int i = 0; i<resultVec.size(); ++i) {
        ASSERT_EQ(resultVec[i].id,i);
        ASSERT_EQ(resultVec[i].cost, i);
    }

}
