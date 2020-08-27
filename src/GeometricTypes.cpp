#include <nlohmann/json.hpp>
#include <optional>
#include <path_finder/graphs/GeometricType.h>
namespace pathFinder {
void from_json(const nlohmann::json &j, BoundingBox &boundingBox) {
  j.at("north").get_to(boundingBox.north);
  j.at("east").get_to(boundingBox.east);
  j.at("south").get_to(boundingBox.south);
  j.at("west").get_to(boundingBox.west);
}
void to_json(nlohmann::json &j, BoundingBox boundingBox) {
  j = nlohmann::json{{"north", boundingBox.north},
                     {"east", boundingBox.east},
                     {"south", boundingBox.south},
                     {"west", boundingBox.west}};
}
std::optional<LatLng> BoundingBox::intersectionPoint(LatLng A, LatLng B, LatLng C, LatLng D) {
  double a1 = B.lng - A.lng;
  double b1 = A.lat - B.lat;
  double c1 = a1 * (A.lat) + b1 * (A.lng);

  double a2 = D.lng - C.lng;
  double b2 = C.lat - D.lat;
  double c2 = a2 * (C.lat) + b2 * (C.lng);

  double determinant = a1 * b2 - a2 * b1;

  if (determinant == 0) {
    return std::nullopt;
  }
  double x = (b2 * c1 - b1 * c2) / determinant;
  double y = (a1 * c2 - a2 * c1) / determinant;
  LatLng intersectionPoint(x, y);

  if (!pointOnLine(A, B, intersectionPoint) || !pointOnLine(C, D, intersectionPoint))
    return std::nullopt;

  return intersectionPoint;
}
LatLng BoundingBox::calcMidPoint(const LatLng &a, const LatLng &b) {
  LatLng vecAB = b - a;
  return a + (vecAB / 2);
}
bool BoundingBox::pointOnLine(LatLng A, LatLng B, LatLng point) {
  BoundingBox boundingBox;
  if (A.lat < B.lat) {
    boundingBox.south = A.lat;
    boundingBox.north = B.lat;
  } else {
    boundingBox.south = B.lat;
    boundingBox.north = A.lat;
  }
  if (A.lng > B.lng) {
    boundingBox.east = A.lng;
    boundingBox.west = B.lng;
  } else {
    boundingBox.east = B.lng;
    boundingBox.west = A.lng;
  }
  return boundingBox.contains(point);
}
void to_json(nlohmann::json &j, LatLng latLng) { j = nlohmann::json{{"lat", latLng.lat}, {"lng", latLng.lng}}; }
void from_json(const nlohmann::json &j, LatLng &latLng) {
  j.at("lat").get_to(latLng.lat);
  j.at("lng").get_to(latLng.lng);
}
}