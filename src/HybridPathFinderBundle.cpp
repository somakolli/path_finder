#include <path_finder/routing/HybridPathFinderBundle.h>
#include <cmath>

namespace pathFinder {
	
namespace {
	

inline double toRadian(double deg) { return (deg*M_PI)/180;}

inline double sqr(double x) { return x*x; } 

inline double distanceTo(double lat0, double lon0, double lat1, double lon1, double earthRadius = 6371000) {
	double ph1 = toRadian(lat0);
	double ph2 = toRadian(lat1);
	double deltaPh = toRadian(lat1-lat0);
	double deltaLambda = toRadian(lon1-lon0);

	double a = sqr(::sin(deltaPh/2)) + ::cos(ph1) * ::cos(ph2) * sqr(sin(deltaLambda/2));
	double c = 2 * ::atan2(::sqrt(a), ::sqrt(1-a));

	return earthRadius * c;
}

inline double distanceTo(LatLng const & src, LatLng const & tgt) {
	return std::abs<double>(distanceTo(src.lat, src.lng, tgt.lat, tgt.lng));
}

}

HybridPathFinderBundle::HybridPathFinderBundle()
{}

HybridPathFinderBundle::~HybridPathFinderBundle()
{}

RoutingResult
HybridPathFinderBundle::getShortestPath(LatLng source, LatLng target) {
	//Get all candidate finders containing source and target
	
	std::vector<std::size_t> sr, tr;
	struct OutIterator {
		OutIterator(std::vector<std::size_t> & dest) : m_dest(std::ref(dest)) {}
		OutIterator(OutIterator const &) = default;
		OutIterator & operator=(OutIterator const &) = default;
		OutIterator & operator=(si_value const & v) {
			m_dest.get().push_back(v.second);
			return *this;
		}
		OutIterator & operator*() { return *this; }
		OutIterator & operator++() { return *this; }
		
		std::reference_wrapper<std::vector<std::size_t>> m_dest;
	};
	
	m_si.query(boost::geometry::index::covers(si_point(source.lat, source.lng)), OutIterator(sr));
	m_si.query(boost::geometry::index::covers(si_point(source.lat, source.lng)), OutIterator(tr));
	
	std::sort(sr.begin(), sr.end());
	std::sort(tr.begin(), tr.end());
	std::vector<std::size_t> br;
	std::set_intersection(sr.begin(), sr.end(), tr.begin(), tr.end(), std::back_inserter(br));
	
	//There may be multiple matching graph. We choose the one that minimizes the combined squared distance
	std::size_t bestId = std::numeric_limits<std::size_t>::max();
	double bestDist = std::numeric_limits<double>::max();
	std::pair<NodeId, NodeId> bestNodeIds(std::numeric_limits<NodeId>::max(), std::numeric_limits<NodeId>::max());
	for(std::size_t id : br) {
		auto const & g = m_d.at(id)->getGraph();
		NodeId srcNodeId = g->getNodeIdFor(source);
		NodeId tgtNodeId = g->getNodeIdFor(target);
		auto const & srcNode = g->getNode(srcNodeId);
		auto const & tgtNode = g->getNode(tgtNodeId);
		double dist = sqr(distanceTo(srcNode.latLng, source))+sqr(distanceTo(tgtNode.latLng, target));
		if (dist < bestDist) {
			bestDist = dist;
			bestNodeIds = std::make_pair(srcNodeId, tgtNodeId);
			bestId = id;
		}
	}
	if (bestId == std::numeric_limits<std::size_t>::max()) {
		return RoutingResult();
	}
	return m_d.at(bestId)->getShortestPath(bestNodeIds.first, bestNodeIds.second);
}

void
HybridPathFinderBundle::add(std::shared_ptr<HybridPathFinder> v) {
	std::size_t id = m_d.size();
	m_d.push_back(v);
	auto const & b = v->getGraph()->boundingBox;
	si_box si_b(si_point(b.south, b.west), si_point(b.north, b.east));
	m_si.insert(std::make_pair(si_b, id));
}

} //end namespace pathFinder
