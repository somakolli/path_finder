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
template<template<class, class> class Vector = std::vector, class OffsetVector = std::vector<OffsetElement>>
class HubLabelStore{
    using CostNodeVector = Vector<CostNode, std::allocator<CostNode>>;
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
    size_t getSpaceConsumption();
};

template<template<class, class> class Vector, class OffsetVector>
HubLabelStore<Vector, OffsetVector>::HubLabelStore(size_t numberOfLabels) {
    forwardOffset.reserve(numberOfLabels);
    backwardOffset.reserve(numberOfLabels);
    while(forwardOffset.size() < numberOfLabels){
        forwardOffset.push_back(OffsetElement());
        backwardOffset.push_back(OffsetElement());
    }
}

template<template<class, class> class Vector, class OffsetVector>
template<typename Id>
void HubLabelStore<Vector, OffsetVector>::store(const std::vector<CostNode> &label, Id id, EdgeDirection direction) {
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

template<template<class, class> class Vector, class OffsetVector>
template<typename Id>
pathFinder::MyIterator<CostNode*> HubLabelStore<Vector, OffsetVector>::retrieve(Id id, EdgeDirection direction){
    if(direction == EdgeDirection::FORWARD){
        auto offsetElement = forwardOffset[id];
        return MyIterator<CostNode*>{&forwardLabels[offsetElement.position], &forwardLabels[offsetElement.position + offsetElement.size]};
    } else {
        auto offsetElement = backwardOffset[id];
        return MyIterator<CostNode*>{&backwardLabels[offsetElement.position], &backwardLabels[offsetElement.position + offsetElement.size]};
    }
}

template<template<class, class> class Vector, class OffsetVector>
size_t HubLabelStore<Vector, OffsetVector>::numberOfLabels() {
    return forwardOffset.size();
}

template<template<class, class> class Vector, class OffsetVector>
HubLabelStore<Vector, OffsetVector>::HubLabelStore(CostNodeVector & forwardLabels, CostNodeVector & backwardLabels, OffsetVector & forwardOffset, OffsetVector & backwardOffset) {
    this->forwardLabels = forwardLabels;
    this->backwardLabels = backwardLabels;
    this->forwardOffset = forwardOffset;
    this->backwardOffset = backwardOffset;
}

template<template<class, class> class Vector, class OffsetVector>
auto &HubLabelStore<Vector, OffsetVector>::getBackwardLabels() {
    return backwardLabels;
}

template<template<class, class> class Vector, class OffsetVector>
auto &HubLabelStore<Vector, OffsetVector>::getForwardLabels() {
    return forwardLabels;
}

template<template<class, class> class Vector, class OffsetVector>
auto &HubLabelStore<Vector, OffsetVector>::getForwardOffset() {
    return forwardOffset;
}

template<template<class, class> class Vector, class OffsetVector>
auto &HubLabelStore<Vector, OffsetVector>::getBackwardOffset() {
    return backwardOffset;
}

template<template<class, class> class Vector, class OffsetVector>
size_t HubLabelStore<Vector, OffsetVector>::getSpaceConsumption() {
    return forwardLabels.size() * sizeof(CostNode) + backwardLabels.size() * sizeof(CostNode);
}
}

#endif //MASTER_ARBEIT_HUBLABELSTORE_H
