//
// Created by sokol on 03.10.19.
//

#include <chrono>
#include <future>
#include "../include/HubLabels.h"
#include "../include/CHDijkstra.h"
#include "algorithm"

pathFinder::HubLabels::HubLabels(pathFinder::CHGraph &graph) : graph(graph) {
    hubLabels.reserve(graph.getNodes().size());
    backHubLabels.reserve(graph.getNodes().size());

    graph.sortByLevel(sortedNodes);

    // calculate node ranges with same level
    std::vector<std::pair<uint32_t, uint32_t >> sameLevelRanges;
    auto currentLevel = sortedNodes.begin()->level;
    for(auto j = 0; j < sortedNodes.size(); ++j) {
        auto i = j;
        while(sortedNodes[j].level == currentLevel && j < sortedNodes.size()){
            ++j;
        }
        sameLevelRanges.emplace_back(i, j+1);
        currentLevel = sortedNodes[j+1].level;
    }

    //initialize vector for labels for each node
    while (hubLabels.size() < graph.getNodes().size()) {
        hubLabels.emplace_back(std::vector<CostNode>());
        backHubLabels.emplace_back(std::vector<CostNode>());
    }
    auto start = std::chrono::steady_clock::now();
    Print("constructing labels...");
    auto launch_policy = std::launch::async;

    // constrauct forward and backward labels
    std::thread thread1(&HubLabels::constructAllLabels, this, 4, sameLevelRanges, EdgeDirection::FORWARD);
    std::thread thread2(&HubLabels::constructAllLabels, this, 4, sameLevelRanges, EdgeDirection::BACKWARD);
    thread1.join();
    thread2.join();
    auto end = std::chrono::steady_clock::now();
    std::cout << "Constructed Labels in  "
              << std::chrono::duration_cast<std::chrono::seconds>(end - start).count()
              << " s" << std::endl;
}

void
pathFinder::HubLabels::constructAllLabels(int maxThreads, const std::vector<std::pair<uint32_t, uint32_t >>& sameLevelRanges,
                                          const pathFinder::EdgeDirection &direction) {
    for(const auto& sameLevelRange : sameLevelRanges) {
        if(direction == pathFinder::EdgeDirection::FORWARD) {
            std:: cout << "Forward  progress: " <<  sameLevelRange.first << "/" << graph.getNodes().size() << std::endl;
        } else {
            std:: cout << "Backward progress: " <<  sameLevelRange.first << "/" << graph.getNodes().size() << std::endl;
        }
        processRange(sameLevelRange, direction);
    }

}

std::vector<pathFinder::CostNode> &pathFinder::HubLabels::getLabels(pathFinder::NodeId nodeId, pathFinder::EdgeDirection direction) {
    switch (direction) {
        case EdgeDirection::FORWARD:
            return hubLabels[nodeId];
        case EdgeDirection::BACKWARD:
            return backHubLabels[nodeId];
    }
    return hubLabels[nodeId];
}

std::optional<pathFinder::Distance> pathFinder::HubLabels::getShortestDistance(pathFinder::NodeId source, pathFinder::NodeId target) {
    if(source >= graph.getNodes().size() || target >= graph.getNodes().size())
        return std::nullopt;
    auto& forwardLabels = getLabels(source, EdgeDirection::FORWARD);
    auto& backwardLabels = getLabels(target, EdgeDirection::BACKWARD);
    NodeId topNode;
    return getDistance(forwardLabels, backwardLabels, topNode);
}

pathFinder::Distance
pathFinder::HubLabels::getDistance(std::vector<CostNode> &forwardLabels, std::vector<CostNode> &backwardLabels, NodeId& topNode) {
    Distance shortestDistance = MAX_DISTANCE;
    topNode = forwardLabels[0].id;
    for(int i = 0, j = 0; i < forwardLabels.size()&& j < backwardLabels.size();) {
        auto& forwardCostNode = forwardLabels[i];
        auto& backwardCostNode = backwardLabels[j];
        if (forwardCostNode.id == backwardCostNode.id ) {
            if(forwardCostNode.cost + backwardCostNode.cost < shortestDistance)
                    shortestDistance = forwardCostNode.cost + backwardCostNode.cost;
            ++j;
            ++i;
            topNode = forwardCostNode.id;
        }
        else if(forwardCostNode.id < backwardCostNode.id) {
            ++i;
        }
        else
            ++j;
    }
    return shortestDistance;
}

void pathFinder::HubLabels::setLabel(pathFinder::NodeId nodeId, pathFinder::EdgeDirection direction) {
    Level level = graph.getLevel(nodeId);
    auto& label = getLabels(nodeId, direction);
    for(const auto& edge: graph.edgesFor(nodeId, direction)) {
        if(level < graph.getLevel(edge.target)) {
            const auto& targetLabel = getLabels(edge.target, direction);
            size_t i = 0;
            for(const auto& [idTarget, distanceTarget] : targetLabel) {
                bool found = false;
                const auto addedDistance = distanceTarget + edge.distance;
                for(;i < label.size(); ++i) {
                    auto&& [id, distance] = label[i];
                    if(id == idTarget) {
                        found = true;
                        if(addedDistance < distance)
                            distance = addedDistance;
                        break;
                    }
                }
                if(!found) {
                    label.emplace_back(idTarget, addedDistance);
                }
            }
        }
    }
    label.emplace_back(nodeId, 0);
    selfPrune(nodeId, direction);
    sortLabel(label);
}

void pathFinder::HubLabels::sortLabel(std::vector<CostNode> &label) {
    std::sort(label.begin(), label.end(), [](const CostNode& node1, const CostNode& node2) {
        return node1.id == node2.id ? node1.cost < node2.cost : node1.id < node2.id;
    });
}

void pathFinder::HubLabels::processRange(std::pair<uint32_t, uint32_t> range, EdgeDirection direction){
    for(auto i = range.first; i < range.second; ++i) {
        setLabel(sortedNodes[i].id, direction);
    }
}



void pathFinder::HubLabels::setLabelAsync(NodeId& begin, const NodeId& end, std::mutex &mutex, EdgeDirection direction) {
    while (true) {
        std::unique_lock<std::mutex> l{mutex};
        if(begin >= end)
            break;
        NodeId nodeToProcess = begin;
        if(nodeToProcess >= graph.getNodes().size())
            break;
        ++begin;
        l.unlock();
        setLabel(sortedNodes[nodeToProcess].id, direction);
    }
}

void pathFinder::HubLabels::processRangeAsync(std::pair<uint32_t, uint32_t> range, pathFinder::EdgeDirection direction, const uint32_t maxNumberOfThreads) {
    std::queue<NodeId> q;
    std::vector<std::thread> threads;
    std::mutex mutex;
    NodeId begin = range.first;
    Print(begin)
    NodeId end = range.second;
    for(int i = 0; i < maxNumberOfThreads; ++i ) {
        threads.emplace_back(std::thread(&HubLabels::setLabelAsync, this, std::ref(begin), std::ref(end), std::ref(mutex), direction));
    }
    for(auto& thread : threads) {
        thread.join();
    }
}

void pathFinder::HubLabels::constructAllLabelsAsync(int maxThreads,
                                                    const std::vector<std::pair<uint32_t, uint32_t >> &sameLevelRanges,
                                                    const pathFinder::EdgeDirection &direction) {
    for(auto& sameLevelRange : sameLevelRanges)
        processRangeAsync(sameLevelRange, direction, maxThreads);
}

void pathFinder::HubLabels::selfPrune(pathFinder::NodeId labelId, pathFinder::EdgeDirection direction) {
    auto& labels = getLabels(labelId, direction);
    auto it = labels.begin();
    while(it != labels.end()) {
        auto& [id, cost] = *it;
        auto& otherLabels = getLabels(labelId, EdgeDirection::BACKWARD);
        if(getShortestDistance(labelId, id) <= cost) {
            labels.erase(it);
        }
        ++it;
    }
}

std::vector<pathFinder::LatLng> pathFinder::HubLabels::getShortestPath(pathFinder::NodeId source, pathFinder::NodeId target) {
    return std::vector<LatLng>();
}
