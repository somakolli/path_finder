//
// Created by sokol on 29.01.20.
//

#ifndef MASTER_ARBEIT_STATIC_H
#define MASTER_ARBEIT_STATIC_H


class Static {
public:
template<typename ItA, typename ItB, typename Container, typename Distance>
static inline void merge(ItA aBegin, ItA aEnd, ItB bBegin, ItB bEnd, Distance distanceToLabel, Container& result) {
    auto i = aBegin;
    auto j = bBegin;
    while(i < aEnd && j < bEnd){
        if(i->id< j->id){
            result.push_back(*i);
            ++i;
        } else if(j->id < i->id){
            result.emplace_back(j->id, j->cost + distanceToLabel, j->previousNode);
            ++j;
        } else {
            if(i->cost < j->cost + distanceToLabel)
                result.emplace_back(*i);
            else
                result.emplace_back(j->id, j->cost + distanceToLabel, j->previousNode);
            ++j;
            ++i;
        }
    }
    while(i != aEnd){
        result.push_back(*i);
        ++i;
    }
    while(j != bEnd) {
        result.emplace_back(j->id, j->cost + distanceToLabel, j->previousNode);
        ++j;
    }
}
};


#endif //MASTER_ARBEIT_STATIC_H
