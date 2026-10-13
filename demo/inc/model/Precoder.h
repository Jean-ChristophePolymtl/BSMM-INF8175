#pragma once

#include "constants.h"
#include "simulation/Simulation.h"
#include <Eigen/Dense>
#include <complex>
#include <raylib.h>

namespace INF8503::model {

/// @brief Calculates the precoding matrix from the channel estimation matrix
/// according to zero force method: W = H^{H} (HH^{H})^(-1)
/// @param channelMat the channel estimation matrix
/// @return the precoding matrix W
Eigen::MatrixXcf zeroForcePrecode(const Eigen::MatrixXcf &channelMat);

Eigen::MatrixXcf calculateZFSVD(const Eigen::MatrixXcf &H);

std::vector<std::vector<Antenna>>
applyPrecode(const Eigen::MatrixXcf &precodeMat,
             const std::vector<Antenna> &antennaVec);

} // namespace INF8503::model
