#pragma once

#include "constants.h"
#include "model/AntennaArray.h"
#include "model/User.h"
#include <Eigen/Dense>
#include <complex>
#include <raylib.h>

namespace INF8503::model {

/// @brief
/// @param lambda
/// @param distance
/// @return
std::complex<float> freeSpaceLOS(float lambda, float distance);

Eigen::MatrixXcf calculateChannelMatrix(const AntennaArray &antennaArray,
                                        const std::vector<User> &usersVec);

} // namespace INF8503::model
