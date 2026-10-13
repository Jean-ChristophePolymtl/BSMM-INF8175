#pragma once

#include "constants.h"
#include "math/Math.h"
#include "model/AntennaArray.h"
#include <Eigen/Dense>
#include <cmath>
#include <iostream>
#include <vector>

namespace INF8503::sim {

std::vector<float>
calculatePower(const std::vector<model::Antenna> &antennaVec);

std::vector<float> calculatePower(const model::AntennaArray &antennaArray);

} // namespace INF8503::sim