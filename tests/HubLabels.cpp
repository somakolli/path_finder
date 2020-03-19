//
// Created by sokol on 27.01.20.
//
#include <gtest/gtest.h>
#include "../include/HubLabelStore.h"
#include "../include/HubLabels.h"
#include "../include/Static.h"
#include <vector>
#include <gmock/gmock-matchers.h>
namespace pathFinder{
TEST(HubLabels, MergeWorks){
    std::vector<pathFinder::CostNode> costNodeVec1 = {
            pathFinder::CostNode{0, 0, 0},
            pathFinder::CostNode{2, 2, 0},
            pathFinder::CostNode{4, 4, 0},
            pathFinder::CostNode{6, 6, 0}
    };
    std::vector<pathFinder::CostNode> costNodeVec2 = {
            pathFinder::CostNode(1, 0, 0),
            pathFinder::CostNode(3, 2, 0),
            pathFinder::CostNode(5, 4, 0),
            pathFinder::CostNode(6, 5, 0),
            pathFinder::CostNode(7, 6, 0),
    };
    std::vector<pathFinder::CostNode> result;
    Static::merge(costNodeVec1.begin(), costNodeVec1.end(), costNodeVec2.begin(), costNodeVec2.end(), 1, result);
    for(int i = 0; i<result.size(); ++i) {
        ASSERT_EQ(result[i].id,i);
        ASSERT_EQ(result[i].cost, i);
    }
}
TEST(HubLabels, EmptyMergeWorks) {
        std::vector<pathFinder::CostNode> costNodeVec1;
        std::vector<pathFinder::CostNode> costNodeVec2 = {
                pathFinder::CostNode(1, 0, 0),
                pathFinder::CostNode(3, 2, 0),
                pathFinder::CostNode(5, 4, 0),
                pathFinder::CostNode(6, 5, 0),
                pathFinder::CostNode(7, 6, 0),
        };
        std::vector<pathFinder::CostNode> result;
        Static::merge(costNodeVec1.begin(), costNodeVec1.end(), costNodeVec2.begin(), costNodeVec2.end(), 1, result);
        for(int i = 0; i<result.size(); ++i) {
            ASSERT_EQ(result[i].id,result[i].cost);
        }
        ASSERT_EQ(costNodeVec2.size(), result.size());
        result.clear();
        Static::merge(costNodeVec2.begin(), costNodeVec2.end(), costNodeVec1.begin(), costNodeVec1.end(), 1,result);
        for(int i = 0; i<result.size(); ++i) {
            ASSERT_EQ(result[i].id-1,result[i].cost);
        }
        ASSERT_EQ(costNodeVec2.size(), result.size());
        result.clear();
        Static::merge(costNodeVec1.begin(), costNodeVec1.end(), costNodeVec1.begin(), costNodeVec1.end(), 1, result);
        ASSERT_EQ(0, result.size());
    }
}