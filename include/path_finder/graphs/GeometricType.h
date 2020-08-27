#ifndef MASTER_ARBEIT_GEOMETRICTYPE_H
#define MASTER_ARBEIT_GEOMETRICTYPE_H
#include <cmath>
#include <path_finder/helper/Math.h>
namespace pathFinder {
struct LatLng {
  using Lat = float;
  using Lng = float;
  Lat lat;
  Lng lng;
  LatLng() = default;
  LatLng(const double lat, const double lng): lat(lat), lng(lng) {}
  bool operator==(LatLng other) const {
    return other.lat == lat && other.lng == lng;
  }
  LatLng operator-(LatLng other) const{
    LatLng newLatLng{};
    newLatLng.lat = lat - other.lat;
    newLatLng.lng = lng - other.lng;
    return newLatLng;
  }
  LatLng operator+(LatLng other) const{
    LatLng newLatLng{};
    newLatLng.lat = lat + other.lat;
    newLatLng.lng = lng + other.lng;
    return newLatLng;
  }
  LatLng operator*(double other) const{
    LatLng newLatLng{};
    newLatLng.lat = other * lat;
    newLatLng.lng = other * lng;
    return newLatLng;
  }
  LatLng operator/(double other) const{
    LatLng newLatLng{};
    newLatLng.lat = lat / other;
    newLatLng.lng = lng / other;
    return newLatLng;
  }
  [[nodiscard]] double length() const {
    return std::sqrt(std::pow(lat, 2) + std::pow(lng, 2));
  }
};

struct BoundingBox {
  double north = 0;
  double east = 0;
  double south = 0;
  double west = 0;

  [[nodiscard]] LatLng southWest() const{
      return LatLng(south, west);
  };
  [[nodiscard]] LatLng northEast() const{
    return LatLng(north, east);
  }
  [[nodiscard]] LatLng southEast() const {
    return LatLng(south, east);
  }
  [[nodiscard]] LatLng northWest() const {
    return LatLng(north, west);
  }
  void setSouthWest(LatLng southWest) {
    south = southWest.lat;
    west = southWest.lng;
  }
  void setNorthEast(LatLng northEast) {
    north = northEast.lat;
    east = northEast.lng;
  }
  [[nodiscard]] bool contains(LatLng point) const{
    float epsilon = 0.00001f;
    return point.lat < north + epsilon &&
           point.lat > south - epsilon &&
           point.lng < east + epsilon &&
           point.lng > west - epsilon;
  }
  [[nodiscard]] LatLng getIntersectionPoint(LatLng midPoint, LatLng outSidePoint) const {
    if (auto point = intersectionPoint(midPoint, outSidePoint, northWest(), northEast());
        point.has_value()) {
      return point.value();
    } else if (auto point1 = intersectionPoint(midPoint, outSidePoint, northEast(), southEast());
               point1.has_value()) {
      return point1.value();
    } else if (auto point2 = intersectionPoint(midPoint, outSidePoint, southEast(), southWest());
               point2.has_value()) {
      return point2.value();
    } else {
      return intersectionPoint(midPoint, outSidePoint, southWest(), northWest()).value();
    }
  }
  static std::optional<LatLng> intersectionPoint(LatLng A, LatLng B, LatLng C, LatLng D);;
  static LatLng calcMidPoint(const LatLng &a, const LatLng &b);
  static bool pointOnLine(LatLng A, LatLng B, LatLng point);
};
void to_json(nlohmann::json& j, BoundingBox boundingBox);
void from_json(const nlohmann::json& j, BoundingBox& boundingBox);

}
#endif // MASTER_ARBEIT_GEOMETRICTYPE_H
