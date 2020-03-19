//
// Created by sokol on 23.01.20.
//

#ifndef MASTER_ARBEIT_HUBLABELSTORE_H
#define MASTER_ARBEIT_HUBLABELSTORE_H

#include <vector>
#include "Graph.h"
namespace pathFinder{
struct OffsetElement {
    size_t position;
    uint32_t size;
};
template<template<class, class> class Vector>
class HubLabelStore{
    using CostNodeVector = Vector<CostNode, std::allocator<CostNode>>;
    using OffsetVector = std::vector<OffsetElement>;
    CostNodeVector forwardLabels;
    CostNodeVector backwardLabels;
    OffsetVector forwardOffset;
    OffsetVector backwardOffset;
public:
    explicit HubLabelStore(size_t numberOfLabels);
    HubLabelStore(CostNodeVector& forwardLabels, CostNodeVector & backwardLabels, OffsetVector & forwardOffset, OffsetVector & backwardOffset);
    template <typename Id>
    void store(const std::vector<CostNode>& label, Id id, EdgeDirection direction);
    template <typename Id>
    pathFinder::MyIterator<CostNode*> retrieve(Id id, EdgeDirection direction);
    size_t numberOfLabels();
    auto& getForwardLabels();
    auto& getBackwardLabels();
    auto& getForwardOffset();
    auto& getBackwardOffset();
};

template<template<class, class> class Vector>
HubLabelStore<Vector>::HubLabelStore(size_t numberOfLabels) {
    forwardOffset.reserve(numberOfLabels);
    backwardOffset.reserve(numberOfLabels);
    while(forwardOffset.size() < numberOfLabels){
        forwardOffset.push_back(OffsetElement());
        backwardOffset.push_back(OffsetElement());
    }
}

template<template<class, class> class Vector>
template<typename Id>
void HubLabelStore<Vector>::store(const std::vector<CostNode> &label, Id id, EdgeDirection direction) {
    if(direction == EdgeDirection::FORWARD){
        forwardOffset[id] = OffsetElement{forwardLabels.size(), (uint32_t)label.size()};
        for(auto element : label){
            forwardLabels.push_back(element);
        }
    } else {
        backwardOffset[id] = OffsetElement{backwardLabels.size(), (uint32_t)label.size()};
        for(auto element : label){
            backwardLabels.push_back(element);
        }
    }
}

template<template<class, class> class Vector>
template<typename Id>
pathFinder::MyIterator<CostNode*> HubLabelStore<Vector>::retrieve(Id id, EdgeDirection direction){
    if(direction == EdgeDirection::FORWARD){
        auto offsetElement = forwardOffset[id];
        return MyIterator<CostNode*>{&forwardLabels[offsetElement.position], &forwardLabels[offsetElement.position + offsetElement.size]};
    } else {
        auto offsetElement = backwardOffset[id];
        return MyIterator<CostNode*>{&backwardLabels[offsetElement.position], &backwardLabels[offsetElement.position + offsetElement.size]};
    }
}

template<template<class, class> class Vector>
size_t HubLabelStore<Vector>::numberOfLabels() {
    return forwardOffset.size();
}

template<template<class, class> class Vector>
HubLabelStore<Vector>::HubLabelStore(CostNodeVector & forwardLabels, CostNodeVector & backwardLabels, OffsetVector & forwardOffset, OffsetVector & backwardOffset) {
    this->forwardLabels = forwardLabels;
    this->backwardLabels = backwardLabels;
    this->forwardOffset = forwardOffset;
    this->backwardOffset = backwardOffset;
}

template<template<class, class> class Vector>
auto &HubLabelStore<Vector>::getBackwardLabels() {
    return backwardLabels;
}

template<template<class, class> class Vector>
auto &HubLabelStore<Vector>::getForwardLabels() {
    return forwardLabels;
}

template<template<class, class> class Vector>
auto &HubLabelStore<Vector>::getForwardOffset() {
    return forwardOffset;
}

template<template<class, class> class Vector>
auto &HubLabelStore<Vector>::getBackwardOffset() {
    return backwardOffset;
}
}

#endif //MASTER_ARBEIT_HUBLABELSTORE_H
