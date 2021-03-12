#pragma once
#include <cmath>
#include <nlohmann/json.hpp>
namespace pathFinder {
struct LatLng {
  using Lat = double;
  using Lng = double;
  Lat lat;
  Lng lng;
  LatLng() = default;
  LatLng(const double lat, const double lng) : lat(lat), lng(lng) {}
  auto operator==(LatLng other) const -> bool { return other.lat == lat && other.lng == lng; }
  auto operator-(LatLng other) const -> LatLng {
    LatLng newLatLng{};
    newLatLng.lat = lat - other.lat;
    newLatLng.lng = lng - other.lng;
    return newLatLng;
  }
  LatLng operator+(LatLng other) const {
    LatLng newLatLng{};
    newLatLng.lat = lat + other.lat;
    newLatLng.lng = lng + other.lng;
    return newLatLng;
  }
  auto operator*(double other) const -> LatLng {
    LatLng newLatLng{};
    newLatLng.lat = other * lat;
    newLatLng.lng = other * lng;
    return newLatLng;
  }
  auto operator/(double other) const -> LatLng {
    LatLng newLatLng{};
    newLatLng.lat = lat / other;
    newLatLng.lng = lng / other;
    return newLatLng;
  }
  [[nodiscard]] auto length() const -> double { return std::sqrt(std::pow(lat, 2) + std::pow(lng, 2)); }
};

struct BoundingBox {
  double north = 0;
  double east = 0;
  double south = 0;
  double west = 0;

  [[nodiscard]] auto southWest() const -> LatLng { return LatLng(south, west); };
  [[nodiscard]] auto northEast() const -> LatLng { return LatLng(north, east); }
  [[nodiscard]] auto southEast() const -> LatLng { return LatLng(south, east); }
  [[nodiscard]] auto northWest() const -> LatLng { return LatLng(north, west); }
  [[maybe_unused]] void setSouthWest(LatLng southWest) {
    south = southWest.lat;
    west = southWest.lng;
  }
  [[maybe_unused]] void setNorthEast(LatLng northEast) {
    north = northEast.lat;
    east = northEast.lng;
  }
  [[nodiscard]] auto contains(LatLng point) const -> bool {
    float epsilon = 0.00001f;
    return point.lat < north + epsilon && point.lat > south - epsilon && point.lng < east + epsilon &&
           point.lng > west - epsilon;
  }
  [[nodiscard]] auto getIntersectionPoint(LatLng midPoint, LatLng outSidePoint) const -> LatLng {
    if (auto point = intersectionPoint(midPoint, outSidePoint, northWest(), northEast()); point.has_value()) {
      return point.value();
    } else if (auto point1 = intersectionPoint(midPoint, outSidePoint, northEast(), southEast()); point1.has_value()) {
      return point1.value();
    } else if (auto point2 = intersectionPoint(midPoint, outSidePoint, southEast(), southWest()); point2.has_value()) {
      return point2.value();
    } else {
      return intersectionPoint(midPoint, outSidePoint, southWest(), northWest()).value();
    }
  }
  static auto intersectionPoint(LatLng A, LatLng B, LatLng C, LatLng D) -> std::optional<LatLng>;
  ;
  static auto calcMidPoint(const LatLng &a, const LatLng &b) -> LatLng;
  static auto pointOnLine(LatLng A, LatLng B, LatLng point) -> bool;
};
void to_json(nlohmann::json &j, BoundingBox boundingBox);
void from_json(const nlohmann::json &j, BoundingBox &boundingBox);

} // namespace pathFinder
