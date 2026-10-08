#include "math/Math.h"

namespace INF8503::math {

float distance(Eigen::Vector2f start, Eigen::Vector2f end) {
  return (end - start).norm();
}

Eigen::Vector2f distanceVector(Eigen::Vector2f start, Eigen::Vector2f end) {
  return (end - start);
}

Eigen::Vector2f polarToCartesian(float radius, float angle,
                                 Eigen::Vector2f center) {
  float angleRad = angle * DEG2RAD;

  return {center[0] + radius * cosf(angle), center[1] - radius * sinf(angle)};
}

Vector2 polarToCartesian(float radius, float angle, Vector2 center) {
  float angleRad = angle * DEG2RAD;

  return {center.x + radius * cosf(angle), center.y - radius * sinf(angle)};
}

std::vector<float> powerVectorToDB(std::vector<float> power) {

  std::vector<float> result;
  float maxValue = *std::max_element(power.begin(), power.end());

  for (auto p : power) {
    result.push_back(powerToDB(p, maxValue));
  }
  return result;
}

float powerToDB(float power, float max) {
  return {10.0f * log10f(power / max)};
}

} // namespace INF8503::math