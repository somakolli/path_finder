#pragma once
#include <path_finder/routing/HybridPathFinder.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <filesystem>

namespace pathFinder {

//A bundle of hybrid path finders, usually connected components of a larger graph
class HybridPathFinderBundle: public PathFinderBase {
public:
	HybridPathFinderBundle();
	HybridPathFinderBundle(HybridPathFinderBundle const&) = default;
	~HybridPathFinderBundle() override;
	HybridPathFinderBundle & operator=(HybridPathFinderBundle const&) = default;
public:
	auto getShortestPath(LatLng source, LatLng target) -> RoutingResult override;
public:
	//If this throws, then you MUST asume that all data is in an inconsistent state
	void add(std::shared_ptr<HybridPathFinder> v);
private:
	using si_point = boost::geometry::model::point<double, 2, boost::geometry::cs::geographic<boost::geometry::degree>>;
	using si_box = boost::geometry::model::box<si_point>;
	using si_value = std::pair<si_box, std::size_t>;
	using si_type = boost::geometry::index::rtree< si_value, boost::geometry::index::rstar<16> >;
private:
	std::vector<std::shared_ptr<HybridPathFinder>> m_d;
	si_type m_si;
};
	
}
