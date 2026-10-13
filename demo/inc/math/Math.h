#pragma once

#include "constants.h"
#include <Eigen/Dense>
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <vector>

namespace INF8503::math {

float distance(const Eigen::Vector2f &start, const Eigen::Vector2f &end);
Eigen::Vector2f distanceVector(const Eigen::Vector2f &start,
                               const Eigen::Vector2f &end);
Eigen::Vector2f polarToCartesian(float radius, float angle,
                                 const Eigen::Vector2f &center);

Vector2 polarToCartesian(float radius, float angle, const Vector2 &center);

std::vector<float> powerVectorToDB(const std::vector<float> &power);
float powerToDB(float power, float max);
} // namespace INF8503::math