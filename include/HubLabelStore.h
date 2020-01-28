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
template<template<class, class> class Vector, class Element, class Allocator>
class HubLabelStore{
    Vector<Element, Allocator> forwardLabels;
    Vector<Element, Allocator> backwardLabels;
    std::vector<OffsetElement> forwardOffset;
    std::vector<OffsetElement> backwardOffset;
public:
    HubLabelStore(size_t numberOfLabels);
    HubLabelStore(std::vector<OffsetElement>& forwardOffset, std::vector<OffsetElement>& backwardOffset);
    HubLabelStore(Vector<Element, Allocator>& forwardLabels, Vector<Element, Allocator>& backwardLabels, std::vector<OffsetElement>& forwardOffset, std::vector<OffsetElement>& backwardOffset);
    template <typename Id>
    void store(const std::vector<Element>& label, Id id, EdgeDirection direction);
    template <typename Id>
    pathFinder::MyIterator<Element*> retrieve(Id id, EdgeDirection direction);
    size_t numberOfLabels();
    Vector<Element, Allocator>& getForwardLabels();
    Vector<Element, Allocator>& getBackwardLabels();
    std::vector<OffsetElement>& getForwardOffset();
    std::vector<OffsetElement>& getBackwardOffset();
};

template<template<class, class> class Vector, class Element, class Allocator>
HubLabelStore<Vector, Element, Allocator>::HubLabelStore(size_t numberOfLabels) {
    forwardOffset.reserve(numberOfLabels);
    backwardOffset.reserve(numberOfLabels);
    while(forwardOffset.size() < numberOfLabels){
        forwardOffset.push_back(OffsetElement());
        backwardOffset.push_back(OffsetElement());
    }
}

template<template<class, class> class Vector, class Element, class Allocator>
template<typename Id>
void HubLabelStore<Vector, Element, Allocator>::store(const std::vector<Element> &label, Id id, EdgeDirection direction) {
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

template<template<class, class> class Vector, class Element, class Allocator>
template<typename Id>
pathFinder::MyIterator<Element*> HubLabelStore<Vector, Element, Allocator>::retrieve(Id id, EdgeDirection direction){
    if(direction == EdgeDirection::FORWARD){
        auto offsetElement = forwardOffset[id];
        return MyIterator<Element*>{&forwardLabels[offsetElement.position], &forwardLabels[offsetElement.position + offsetElement.size]};
    } else {
        auto offsetElement = backwardOffset[id];
        return MyIterator<Element*>{&backwardLabels[offsetElement.position], &backwardLabels[offsetElement.position + offsetElement.size]};
    }
}

template<template<class, class> class Vector, class Element, class Allocator>
size_t HubLabelStore<Vector, Element, Allocator>::numberOfLabels() {
    return forwardOffset.size();
}

template<template<class, class> class Vector, class Element, class Allocator>
HubLabelStore<Vector, Element, Allocator>::HubLabelStore(Vector<Element, Allocator>& forwardLabels, Vector<Element, Allocator>& backwardLabels, std::vector<OffsetElement>& forwardOffset, std::vector<OffsetElement>& backwardOffset) {
    this->forwardLabels = forwardLabels;
    this->backwardLabels = backwardLabels;
    this->forwardOffset = forwardOffset;
    this->backwardOffset = backwardOffset;
}

template<template<class, class> class Vector, class Element, class Allocator>
Vector<Element, Allocator> &HubLabelStore<Vector, Element, Allocator>::getBackwardLabels() {
    return backwardLabels;
}

template<template<class, class> class Vector, class Element, class Allocator>
Vector<Element, Allocator> &HubLabelStore<Vector, Element, Allocator>::getForwardLabels() {
    return forwardLabels;
}

template<template<class, class> class Vector, class Element, class Allocator>
std::vector<OffsetElement> &HubLabelStore<Vector, Element, Allocator>::getForwardOffset() {
    return forwardOffset;
}

template<template<class, class> class Vector, class Element, class Allocator>
std::vector<OffsetElement> &HubLabelStore<Vector, Element, Allocator>::getBackwardOffset() {
    return backwardOffset;
}

template<template<class, class> class Vector, class Element, class Allocator>
HubLabelStore<Vector, Element, Allocator>::HubLabelStore(std::vector<OffsetElement>& forwardOffset, std::vector<OffsetElement>& backwardOffset): forwardOffset(forwardOffset), backwardOffset(backwardOffset){

}
}

#endif //MASTER_ARBEIT_HUBLABELSTORE_H
