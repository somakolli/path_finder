//
// Created by sokol on 29.01.20.
//

#ifndef MASTER_ARBEIT_STATIC_H
#define MASTER_ARBEIT_STATIC_H


class Static {
public:
template<typename ItA, typename ItB, typename Container, typename Distance, typename Less, typename DistanceAdder, typename CostComparer>
static inline void merge(ItA aBegin, ItA aEnd, ItB bBegin, ItB bEnd, Distance distanceToLabel,
        Less l, DistanceAdder distanceAdder, CostComparer costComparer, Container& result) {
    auto i = aBegin;
    auto j = bBegin;
    while(i < aEnd && j < bEnd){
        if(l(*i, *j)){
            ++i;
            result.push_back(*i);
        } else if(l(*j,*i)){
            ++j;
            result.push_back(distanceAdder(*j, distanceToLabel));
        } else {
            if(costComparer(*i, distanceAdder(*j, distanceToLabel)))
                result.push_back(*i);
            else
                result.push_back(distanceAdder(*j, distanceToLabel));
            ++j;
            ++i;
        }
    }
    while(i != aEnd){
        result.push_back(*i);
        ++i;
    }
    while(j != bEnd) {
        result.push_back(distanceAdder(*j, distanceToLabel));
        ++j;
    }
}
};


#endif //MASTER_ARBEIT_STATIC_H
