#pragma once
#include <algorithm>
#include <execution>
#include <iostream>
#include <mutex>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <thread>

#include <path_finder/storage/CellIdStore.h>
#include <path_finder/graphs/CHGraph.h>

namespace pathFinder {

inline namespace std_threads_variant {

class OscarIntegrator {
public:
template <typename GeoPoint, typename CellIdsForEdge>
static void writeCellIdsForEdges(const CHGraph &graph, CellIdStore &cellIdStore, CellIdsForEdge edge2CellIds) {
	struct State {
		const CHGraph &graph;
		CellIdStore &cellIdStore;
		CellIdsForEdge & edge2CellIds;
		std::vector<std::atomic<uint64_t>> finishedNodes;
		std::atomic<std::size_t> progress{0};
		std::size_t edgeProgress{0};
		const std::size_t numberOfNodes;
		std::mutex cellIdStoreLock;
		
		State(const CHGraph &graph, CellIdStore &cellIdStore, CellIdsForEdge & edge2CellIds) :
		graph(graph),
		cellIdStore(cellIdStore),
		edge2CellIds(edge2CellIds),
		finishedNodes(graph.getNumberOfNodes() / 64 + 1),
		numberOfNodes(graph.getNumberOfNodes())
		{
			for(auto & x : finishedNodes) {
				x = 0;
			}
		}
		
		bool takeNode(NodeId nodeId) {
			std::size_t chunk = nodeId / 64;
			std::size_t bit = nodeId % 64;
			uint64_t flag = static_cast<uint64_t>(1) << bit;
			uint64_t prev = finishedNodes.at(chunk).fetch_or(flag, std::memory_order_relaxed);
			if (prev & flag) { //already taken
				return false;
			}
			progress.fetch_add(1, std::memory_order_relaxed);
			return true;
		}
	} state(graph, cellIdStore, edge2CellIds);

	//A worker explores the graph depth-first
	//For each node we store the hint given by the cellId operator and reuse it when backtracking
	//Since multiple workers work in parallel we have to make sure that a node is not visited twice
	//This is tracked in the finishedNodes vector
	//If the node queue is empty then we choose a new random node and try to explore from there
	//If the node is already taken then we sample all nodes from the beginning to make sure that each node is visited
	struct Worker {
		struct DFSElement {
			decltype(graph.edgesFor(0, EdgeDirection::FORWARD)) edges;
			decltype(graph.edgesFor(0, EdgeDirection::FORWARD).begin()) it;
			typename CellIdsForEdge::Hint hint;
			DFSElement(decltype(edges) const & edges, decltype(hint) hint) :
			edges(edges),
			it(edges.begin()),
			hint(hint)
			{}
		};
		State *state;
		CellIdsForEdge edge2CellIds;
		std::vector<DFSElement> stack;
		std::default_random_engine rndgen;
		std::uniform_int_distribution<uint32_t> nodeIdRnd;
		std::vector< std::pair<std::size_t, std::vector<uint32_t>> > buffer; //edgeId -> cellIds
		std::size_t apxBufferSizeInBytes{0};
		Worker(State *state) :
		state(state),
		edge2CellIds(state->edge2CellIds),
		nodeIdRnd(0, state->numberOfNodes-1)
		{}
		~Worker() {
			flush();
		}
		void operator()() {
			while (true) {
				if (state->progress >= state->numberOfNodes) {
					break;
				}
				// first try a random nodeId if that fails, sample all
				NodeId nid = nodeIdRnd(rndgen);
				if (!state->takeNode(nid)) { //try all from the beginning
					nid = state->numberOfNodes;
					for(std::size_t i(0), s(state->finishedNodes.size()); i < s && nid >= state->numberOfNodes; ++i) {
						uint64_t tmp = state->finishedNodes[i].load(std::memory_order_relaxed);
						while(tmp != std::numeric_limits<uint64_t>::max() && nid >= state->numberOfNodes) {
							tmp = ~tmp;
							static_assert(std::is_same<unsigned long, decltype(tmp)>::value);
							//tmp cannot be 0 otherwise we wouldn't be here
							nid = i*64 + 63-__builtin_clzl(tmp);
							if (!state->takeNode(nid)) {
								nid = state->numberOfNodes;
								tmp = state->finishedNodes[i].load(std::memory_order_relaxed);
							}
						}
					}
					if (nid >= state->numberOfNodes) {
						//No node found, this means that all were processed during search for a new one
						flush();
						assert(state->progress == state->numberOfNodes);
						break;
					}
				}
				assert(nid < state->numberOfNodes);
				{
					auto node = state->graph.getNode(nid);
					typename CellIdsForEdge::Hint fh;
					//Get the face hint for this node
					edge2CellIds(node.latLng.lat, node.latLng.lng, node.latLng.lat, node.latLng.lng, fh);
					stack.emplace_back(state->graph.edgesFor(nid, EdgeDirection::FORWARD), fh);
				}
				while (stack.size()) {
					//Check if we're at the end of our edge list
					if (stack.back().it == stack.back().edges.end()) {
						stack.pop_back();
						continue;
					}
					//expand next edge
					auto edge = *stack.back().it;
					++stack.back().it; //move to next edge immediately in case we need to skip this edge
					if (edge.child1.has_value()) { //skip shortcut edges
						continue;
					}
					const auto sourceNode = state->graph.getNode(edge.source);
					const auto targetNode = state->graph.getNode(edge.target);
					auto hint = stack.back().hint;
					auto edgePos = state->graph.getEdgePosition(edge, EdgeDirection::FORWARD);
					if (edgePos) {
						buffer.emplace_back(
							edgePos.value(),
							edge2CellIds(   sourceNode.latLng.lat, sourceNode.latLng.lng,
											targetNode.latLng.lat, targetNode.latLng.lng,
											hint
										)
						);
					}
					else {
						std::cerr << "BUG: Edge " << edge << " has no forward position" << std::endl;
					}
					apxBufferSizeInBytes += sizeof(typename std::decay_t<decltype(buffer)>::value_type) + buffer.back().second.size()*sizeof(uint32_t);
					//check if we can descend into the node
					if (state->takeNode(targetNode.id)) {
						stack.emplace_back(state->graph.edgesFor(targetNode.id, EdgeDirection::FORWARD), hint);
					}
					//check if we need to flush our buffer
					if (apxBufferSizeInBytes > 128*1024*1024) {
						flush();
					}
				}
			}
		}
		void flush() {
			if (buffer.size()) {
				std::lock_guard<std::mutex> lck(state->cellIdStoreLock);
				for(auto & x : buffer) {
					state->cellIdStore.storeCellIds(x.first, std::move(x.second));
				}
				state->edgeProgress += buffer.size();
				buffer.clear();
				apxBufferSizeInBytes = 0;
			}
		}
	};
	std::cout << "Computing cell ids for regular edges..." << std::flush;
	if (1) {
		std::vector<std::thread> threads;
		for(std::size_t i(0), s(std::thread::hardware_concurrency()); i < s; ++i) {
			threads.emplace_back(Worker(&state));
		}
		for(auto & x : threads) {
			x.join();
		}
	}
	else {
		Worker w(&state);
		w();
	}
	std::cout << "done" << std::endl;
	std::cout << "Found " << state.edgeProgress << " regular edges our of a total of " << graph.getNumberOfEdges() << std::endl;
	
	struct PendingEdge {
		uint8_t pending = 2;
		uint32_t edgePos; 
	};
	int progress = 0;
	const auto &edges = graph.getEdges();
	std::unordered_map<uint32_t, uint8_t> pendingChildren; //shortcut -> num unfinished children
	std::unordered_multimap<uint32_t, uint32_t> edgeParents; //maps from shortcut-children->shortcuts, note that an edge may have multiple parents
	std::unordered_set<uint32_t> edgesWithChildren; //contains all shortcuts for which both children have their cellids computed
	
	//first get all shortcuts and set them as parent of their respective children
	std::cout << "Computing shortcut dependency tree..." << std::flush;
	for (uint32_t i(0), s(graph.getNumberOfEdges()); i < s; ++i) {
		const auto &edge = edges[i];
		if (edge.child1.has_value()) {
			assert(edge.child1 != edge.child2);
			edgeParents.emplace(edge.child1.value(), i);
			edgeParents.emplace(edge.child2.value(), i);
			pendingChildren[i] = 2;
		}
		else {
			state.edgeProgress -= 1;
		}
	}
	std::cout << "done" << std::endl;
	assert(!state.edgeProgress);
	
	//Find shortcuts that have a regular edge as a parent
	std::cout << "Computing cell ids for shortcut edges..." << std::flush;
	for (uint32_t i(0), s(graph.getNumberOfEdges()); i < s; ++i) {
		auto const & edge = edges[i];
		if (edge.child1.has_value()) {
			continue;
		}
		if (!edgeParents.count(i)) {
			continue;
		}
		
		auto parents = edgeParents.equal_range(i);
		for(auto it(parents.first); it != parents.second; ++it) {
			auto parent = it->second;
			auto & x = pendingChildren.at(parent);
			x -= 1;
			assert(x <= 2);
			if (x == 0) { //our parent has all of its children set
				edgesWithChildren.insert(parent);
				pendingChildren.erase(parent);
			}
		}
	}
	//now compute the union of the cellids of the children
	//We do this recursivley kind of bottom up, but the unordered_set defines the actual order
	//however this should not reduce the performance since the edges have static dependencies and
	//thus the work to be done does not depend on the order we do it (apart from cache issues)
	while (edgesWithChildren.size()) {
		uint32_t edgeId = *edgesWithChildren.begin();
		auto const & edge = edges[edgeId];
		//get the cellids of the children, these are sorted, thus we can use std::set_union
		auto c1cids = cellIdStore.getCellIds(edge.child1.value());
		auto c2cids = cellIdStore.getCellIds(edge.child2.value());
		assert(std::is_sorted(c1cids.begin(), c1cids.end()));
		assert(std::is_sorted(c2cids.begin(), c2cids.end()));
		std::vector<std::decay_t<decltype(c2cids)>::value_type> ecids;
		std::set_union(c1cids.begin(), c1cids.end(), c2cids.begin(), c2cids.end(), std::back_inserter(ecids));
		cellIdStore.storeCellIds(edgeId, std::move(ecids));
		
		//take care of parent
		auto parents = edgeParents.equal_range(edgeId);
		for(auto it(parents.first); it != parents.second; ++it) {
			auto parent = it->second;
			auto & x = pendingChildren.at(parent);
			x -= 1;
			assert(x <= 2);
			if (x == 0) { //our parent has all of its children set
				edgesWithChildren.insert(parent);
				pendingChildren.erase(parent);
			}
		}
		//remove ourself
		edgesWithChildren.erase(edgeId);
	}
	std::cout << "done" << std::endl;
	if (pendingChildren.size()) {
		throw std::runtime_error("Could not compute all shortcut cellids");
	}
	cellIdStore.shrink_to_fit();
}
};

}

namespace omp_variant {
class OscarIntegrator {
public:
  template <typename GeoPoint, typename CellIdsForEdge, typename KVStore>
  static void writeCellIdsForEdges(const CHGraph &graph, CellIdStore &cellIdStore,
                                   KVStore &store) {
    const auto& edges = graph.getEdges();
    int progress = 0;
#pragma omp parallel for default(none) shared(edges, graph, cellIdStore, store, progress, std::cout) num_threads(16)
    for (int i = 0; i < graph.getNumberOfNodes(); ++i) {
      CellIdsForEdge cellIdsForEdge(store);
      for(const auto& edge : graph.edgesFor(i, EdgeDirection::FORWARD)) {
        if (edge.child1.has_value()) {
          continue;
        }
        std::vector<uint32_t> cellIds;
        const auto sourceNode = graph.getNode(edge.source);
        const auto targetNode = graph.getNode(edge.target);
        GeoPoint sourcePoint;
        sourcePoint.lat() = sourceNode.latLng.lat;
        sourcePoint.lon() = sourceNode.latLng.lng;
        GeoPoint targetPoint;
        targetPoint.lat() = targetNode.latLng.lat;
        targetPoint.lon() = targetNode.latLng.lng;

        try {
          auto cellIdsEdge = cellIdsForEdge(sourcePoint, targetPoint);
          cellIds.insert(cellIds.end(), cellIdsEdge.begin(), cellIdsEdge.end());
        } catch (std::exception &e) {
        }
        cellIds.erase(std::remove(cellIds.begin(), cellIds.end(), 4294967295), cellIds.end());
 #pragma omp critical
        {
          cellIdStore.storeCellIds(graph.getEdgePosition(edge, EdgeDirection::FORWARD).value(), cellIds);
          ++progress;
          if (false) //progress % 1000 == 0)
            std::cout << "progress: " << progress << "/" << graph.getNumberOfEdges() << '\n';
          // std::cout << "count: " << cellIds.size() << '\n';
        }
      }
    }
    for (int i = 0; i < graph.getNumberOfEdges(); ++i) {
      const auto &edge = edges[i];
      if (edge.child1.has_value()) {
        const auto fullEdges = graph.getPathFromShortcut(edge, 0);
        std::vector<size_t> fullEdgeIds;
        fullEdgeIds.reserve(fullEdges.size());
        for (const auto fullEdge : fullEdges) {
          fullEdgeIds.emplace_back(graph.getEdgePosition(fullEdge, EdgeDirection::FORWARD).value());
        }
        auto fullCellIds = cellIdStore.getCellIds(fullEdgeIds);
        sort(fullCellIds.begin(), fullCellIds.end());
        (fullCellIds).erase(unique(fullCellIds.begin(), fullCellIds.end()), fullCellIds.end());
        cellIdStore.storeCellIds(i, fullCellIds);
        ++progress;
        if (progress % 1000 == 0)
          std::cout << "progress: " << progress << "/" << graph.getNumberOfEdges() << '\n';
      }
    }
    cellIdStore.shrink_to_fit();
  }
};
}
} // namespace pathFinder
