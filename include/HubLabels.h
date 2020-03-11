//
// Created by sokol on 03.10.19.
//

#ifndef ALG_ENG_PROJECT_HUBLABELS_H
#define ALG_ENG_PROJECT_HUBLABELS_H

#include <vector>
#include <set>
#include <map>
#include "Graph.h"
#include "CHGraph.h"
#include "thread"
#include <boost/container/small_vector.hpp>
#include "queue"
#include "PathFinderBase.h"
#include "HubLabelStore.h"
#include "MmapVector.h"
#include "Static.h"
#include <boost/filesystem/fstream.hpp>
#include <iostream>

namespace pathFinder {

struct DistanceComparer {
    inline bool operator()(CostNode const & a, CostNode const & b) const {
        return a.cost < b.cost;
    }
};

struct DistanceAdder {
    inline CostNode operator()(CostNode const & costNode, Distance distance) const {
        return CostNode(costNode.id, costNode.cost + distance, costNode.previousNode);
    }
};

struct Less {
    inline bool operator()(CostNode const & a, CostNode const & b) const {
        return a.id < b.id;
    }
};

typedef std::vector<CostNode> costNodeVec_t;
template< typename HubLabelStore, typename Graph>
class HubLabels : public PathFinderBase {
private:
    Level labelsUntilLevel = 0;
    HubLabelStore hubLabelStore;
    std::vector<CHNode> sortedNodes;
    std::vector<Distance> cost;
    std::vector<NodeId> visited;
    Graph& graph;
    std::vector<CostNode> calcLabel(NodeId nodeId, EdgeDirection direction);
    void processRange(std::pair<uint32_t, uint32_t> range, EdgeDirection direction);
    void processRangeParallel(std::pair<uint32_t, uint32_t> range, EdgeDirection direction);
    void constructAllLabels(const std::vector<std::pair<uint32_t, uint32_t >>& sameLevelRanges, int maxLevel, int minLevel);
    void selfPrune(std::vector<CostNode>& labels, pathFinder::NodeId nodeId, pathFinder::EdgeDirection direction);
    costNodeVec_t calcLabel(NodeId source, EdgeDirection direction, double& searchTime, double& mergeTime);
    std::optional<Distance> getShortestDistancePrep(NodeId source, NodeId target);
public:
    HubLabels(Graph &graph, Level level);
    HubLabels(Graph &graph, Level level, std::vector<CHNode>& sortedNodes, std::vector<Distance>& cost);
    HubLabels(Graph &graph, Level level, HubLabelStore& hubLabelStore);
    HubLabelStore& getHublabelStore();
    std::optional<Distance> getShortestDistance(NodeId source, NodeId target) override;
    void setMinLevel(Level level);
    std::optional<Distance> getShortestDistance(NodeId source, NodeId target, double& searchTime, double& mergeTime, double& lookUpTime);
    static std::optional<Distance> getShortestDistance(MyIterator<CostNode*> forwardLabels, MyIterator<CostNode*> backwardLabels, NodeId& nodeId);
    static void sortLabel(costNodeVec_t &label);
    std::vector<LatLng> getShortestPath(NodeId source, NodeId target) override;
    void writeToFile(boost::filesystem::path filePath);
    size_t getSpaceConsumption(){
        size_t spaceConsumption = 0;
        return spaceConsumption;
    }
};

template< typename HubLabelStore, typename Graph>
pathFinder::HubLabels<HubLabelStore, Graph>::HubLabels(Graph &graph, Level level) : graph(graph), labelsUntilLevel(level), hubLabelStore(graph.getNodes().size()) {
    cost.reserve(graph.getNodes().size());
    while(cost.size() < graph.getNodes().size())
        cost.push_back(MAX_DISTANCE);

    graph.sortByLevel(sortedNodes);

    // calculate node ranges with same level
    std::vector<std::pair<uint32_t, uint32_t >> sameLevelRanges;
    auto maxLevel = sortedNodes.begin()->level;
    auto currentLevel = sortedNodes.begin()->level;
    for(auto j = 0; j < sortedNodes.size(); ++j) {
        auto i = j;
        while(sortedNodes[j].level == currentLevel && j < sortedNodes.size()){
            ++j;
        }
        sameLevelRanges.emplace_back(i, j+1);
        currentLevel--;
    }
    auto start = std::chrono::steady_clock::now();
    std::cout << "constructing labels..." << std::endl;
    constructAllLabels(sameLevelRanges, maxLevel, labelsUntilLevel);

    auto end = std::chrono::steady_clock::now();
    std::cout << "Constructed Labels in  "
              << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
              << " s" << std::endl;
}

template< typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::constructAllLabels(const std::vector<std::pair<uint32_t, uint32_t >>& sameLevelRanges, int maxLevel, int minLevel) {
    int currentLevel = maxLevel;
    for(const auto& sameLevelRange : sameLevelRanges) {
        std:: cout << "constructing level: " << currentLevel << std::endl;
        processRangeParallel(sameLevelRange, EdgeDirection::FORWARD);
        processRangeParallel(sameLevelRange, EdgeDirection::BACKWARD);
        if(--currentLevel < minLevel)
            break;
    }
}

template< typename HubLabelStore, typename Graph>
std::optional<pathFinder::Distance> pathFinder::HubLabels<HubLabelStore, Graph>::getShortestDistance(pathFinder::NodeId source, pathFinder::NodeId target, double& searchTime, double& mergeTime, double& lookUpTime) {
    if(source >= graph.getNodes().size() || target >= graph.getNodes().size())
        return std::nullopt;
    auto forwardLabels = calcLabel(source, EdgeDirection::FORWARD, searchTime, mergeTime);
    auto backwardLabels = calcLabel(target, EdgeDirection::BACKWARD, searchTime, mergeTime);
    NodeId topNode;
    auto start = std::chrono::high_resolution_clock::now();
    auto d = getShortestDistance(MyIterator(forwardLabels.begin().base(), forwardLabels.end().base()), MyIterator(backwardLabels.begin().base(), backwardLabels.end().base()), topNode);
    auto finish = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    lookUpTime += elapsed.count();
    return d;
}

template< typename HubLabelStore, typename Graph>
std::optional<pathFinder::Distance> pathFinder::HubLabels<HubLabelStore, Graph>::getShortestDistancePrep(pathFinder::NodeId source, pathFinder::NodeId target) {
    if(source >= graph.getNodes().size() || target >= graph.getNodes().size())
        return std::nullopt;
    auto forwardLabels = hubLabelStore.retrieve(source, EdgeDirection::FORWARD);
    auto backwardLabels = hubLabelStore.retrieve(target, EdgeDirection::BACKWARD);
    NodeId topNode;
    return getShortestDistance(forwardLabels, backwardLabels, topNode);
}

template< typename HubLabelStore, typename Graph>
std::optional<pathFinder::Distance>
pathFinder::HubLabels<HubLabelStore, Graph>::getShortestDistance(MyIterator<CostNode*> forwardLabels, MyIterator<CostNode*> backwardLabels, NodeId& nodeId) {
    Distance shortestDistance = MAX_DISTANCE;
    nodeId = 0;
    auto i = forwardLabels.begin();
    auto j = backwardLabels.begin();
    while(i != forwardLabels.end()&& j != backwardLabels.end()) {
        auto& forwardCostNode = *i;
        auto& backwardCostNode = *j;
        if (forwardCostNode.id == backwardCostNode.id ) {
            if(forwardCostNode.cost + backwardCostNode.cost < shortestDistance)
                shortestDistance = forwardCostNode.cost + backwardCostNode.cost;
            ++j;
            ++i;
            nodeId = forwardCostNode.id;
        }
        else if(forwardCostNode.id < backwardCostNode.id)
            ++i;
        else
            ++j;
    }
    return shortestDistance;
}
template< typename HubLabelStore, typename Graph>
std::vector<pathFinder::CostNode> pathFinder::HubLabels<HubLabelStore, Graph>::calcLabel(pathFinder::NodeId nodeId, pathFinder::EdgeDirection direction) {
    Level level = graph.getLevel(nodeId);
    costNodeVec_t label;
    costNodeVec_t newLabel;
    label.reserve(1000);
    for(const auto& edge: graph.edgesFor(nodeId, direction)) {
        if(level < graph.getLevel(edge.target)) {
            auto targetLabel = hubLabelStore.retrieve(edge.target, direction);
            newLabel.clear();
            newLabel.reserve(targetLabel.size() + label.size());
            Static::merge(label.begin(), label.end(), targetLabel.begin(), targetLabel.end(), edge.distance, Less(), DistanceAdder(), DistanceComparer(), newLabel);
            label = std::move(newLabel);
        }
    }
    label.emplace_back(nodeId, 0, nodeId);
    selfPrune(label, nodeId, direction);
    sortLabel(label);
    return label;
}

template< typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::sortLabel(costNodeVec_t &label) {
    std::sort(label.begin(), label.end(), [](const CostNode& node1, const CostNode& node2) {
        return node1.id == node2.id ? node1.cost < node2.cost : node1.id < node2.id;
    });
}

template< typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::processRange(std::pair<uint32_t, uint32_t> range, EdgeDirection direction){
    for(auto i = range.first; i < range.second; ++i) {
        auto label = calcLabel(sortedNodes[i].id, direction);
        hubLabelStore.store(label, sortedNodes[i].id, direction);
    }
}
template<typename HubLabelStore, typename Graph>
void
HubLabels<HubLabelStore, Graph>::processRangeParallel(std::pair<uint32_t, uint32_t> range, EdgeDirection direction) {
    std::vector<costNodeVec_t> labelsForRange;
    labelsForRange.reserve(range.second-range.first);
    for(auto i = range.first; i < range.second; ++i)
        labelsForRange.emplace_back(costNodeVec_t());
    #pragma omp parallel for
    for(auto i = range.first; i < range.second; ++i) {
        labelsForRange[i - range.first] = calcLabel(sortedNodes[i].id, direction);
    }
    int i = 0;
    for(auto& label: labelsForRange){
        hubLabelStore.store(label, sortedNodes[range.first + i++].id, direction);
        label.clear();
    }
}
template< typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::selfPrune(std::vector<CostNode>& labels, pathFinder::NodeId nodeId, pathFinder::EdgeDirection direction) {
    bool forward = (direction == EdgeDirection::FORWARD);
    for(int i = labels.size()-1; i > 0; --i) {
        auto& [id, cost, previousNode] = labels[i];
        const auto& otherLabels = hubLabelStore.retrieve(id, (EdgeDirection) !direction);
        auto d = forward ? getShortestDistancePrep(nodeId, id) : getShortestDistancePrep(id, nodeId);
        if(d < cost){
            labels[i] = labels[labels.size()-1];
            labels.pop_back();
        }
    }
}
    template< typename HubLabelStore, typename Graph>


std::vector<pathFinder::LatLng> pathFinder::HubLabels<HubLabelStore, Graph>::getShortestPath(pathFinder::NodeId source, pathFinder::NodeId target) {
    return std::vector<LatLng>();
}

template< typename HubLabelStore, typename Graph>
pathFinder::costNodeVec_t pathFinder::HubLabels<HubLabelStore, Graph>::calcLabel(NodeId source, EdgeDirection direction, double& searchTime, double& mergeTime){
    const auto& sourceLabel = hubLabelStore.retrieve(source, direction);
    if(!sourceLabel.empty()){
        costNodeVec_t vec;
        for(const auto entry : sourceLabel)
            vec.push_back(entry);
        return vec;
    }

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<CostNode> settledNodes;
    std::vector<NodeId> labelsToCollect;
    for(auto nodeId: visited)
        cost[nodeId] = MAX_DISTANCE;
    visited.clear();
    std::priority_queue<CostNode> q;
    q.emplace(source, 0, source);
    cost[source] = 0;
    visited.emplace_back(source);
    while(!q.empty()) {
        auto costNode = q.top();
        q.pop();
        if(costNode.cost > cost[costNode.id])
            continue;
        settledNodes.emplace_back(costNode.id, costNode.cost, costNode.previousNode);
        auto currentNode = graph.getNodes()[costNode.id];
        if(currentNode.level >= labelsUntilLevel){
            labelsToCollect.emplace_back(costNode.id);
            continue;
        }
        for(const auto& edge: graph.edgesFor(costNode.id, direction)) {
            if(graph.getLevel(edge.source) > graph.getLevel(edge.target))
                continue;
            auto addedCost = costNode.cost + edge.distance;
            if(addedCost < cost[edge.target]) {
                visited.emplace_back(edge.target);
                cost[edge.target] = addedCost;
                q.emplace(edge.target, addedCost, edge.source);
            }
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    searchTime += elapsed.count();
    start = std::chrono::high_resolution_clock::now();
    sortLabel(settledNodes);
    for(auto id : labelsToCollect) {
        std::vector<CostNode> resultVec;
        auto targetLabel = hubLabelStore.retrieve(id, direction);
        Static::merge(settledNodes.begin(), settledNodes.end(),  targetLabel.begin(), targetLabel.end(), cost[id], Less(), DistanceAdder(), DistanceComparer(), resultVec);
        settledNodes = resultVec;
    }
    finish = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
    mergeTime += elapsed.count();
    return settledNodes;
}

template< typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::setMinLevel(pathFinder::Level level) {
    this->labelsUntilLevel = level;
}
template< typename HubLabelStore, typename Graph>
void pathFinder::HubLabels<HubLabelStore, Graph>::writeToFile(boost::filesystem::path filePath){
    //TODO implement maybe
}

template< typename HubLabelStore, typename Graph>
std::optional<pathFinder::Distance>
pathFinder::HubLabels<HubLabelStore, Graph>::getShortestDistance(pathFinder::NodeId source, pathFinder::NodeId target) {
    double mergeTime = 0;
    double searchTime = 0;
    double bla = 0;
    return getShortestDistance(source, target, searchTime, mergeTime, bla);
}

template< typename HubLabelStore, typename Graph>
pathFinder::HubLabels<HubLabelStore, Graph>::HubLabels(Graph &graph, pathFinder::Level level,
                                                HubLabelStore &hubLabelStore) : graph(graph), labelsUntilLevel(level), hubLabelStore(hubLabelStore) {
    graph.sortByLevel(sortedNodes);
    cost.reserve(graph.getNodes().size());
    while(cost.size() < graph.getNodes().size())
        cost.push_back(MAX_DISTANCE);
}

template< typename HubLabelStore, typename Graph>
HubLabelStore &HubLabels<HubLabelStore, Graph>::getHublabelStore() {
    return hubLabelStore;
}

template<typename HubLabelStore, typename Graph>
HubLabels<HubLabelStore, Graph>::HubLabels(Graph &graph, Level level, std::vector<CHNode>& sortedNodes, std::vector<Distance>& cost): graph(graph), labelsUntilLevel(level), hubLabelStore(graph.getNodes().size()) {
    this->graph = graph;
    this->labelsUntilLevel = level;
    this->sortedNodes = sortedNodes;
    this->cost = cost;

    // calculate node ranges with same level
    std::vector<std::pair<uint32_t, uint32_t >> sameLevelRanges;
    auto maxLevel = sortedNodes.begin()->level;
    auto currentLevel = sortedNodes.begin()->level;
    for(int j = 0; j < sortedNodes.size(); ++j) {
        auto i = j;
        while(sortedNodes[j].level == currentLevel && j < sortedNodes.size()){
            ++j;
        }
        sameLevelRanges.emplace_back(i, j+1);
        currentLevel--;
    }
    auto start = std::chrono::steady_clock::now();
    std::cout << "constructing labels..." << std::endl;
    constructAllLabels(sameLevelRanges, maxLevel, labelsUntilLevel);

    // construct forward and backward labels
    auto end = std::chrono::steady_clock::now();
    std::cout << "Constructed Labels in  "
              << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
              << " s" << std::endl;
}


}
#endif //ALG_ENG_PROJECT_HUBLABELS_H
