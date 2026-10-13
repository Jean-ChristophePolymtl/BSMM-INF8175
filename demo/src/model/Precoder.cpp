
#include "model/Precoder.h"

namespace INF8503::model {

Eigen::MatrixXcf zeroForcePrecode(const Eigen::MatrixXcf &channelMat) {

  Eigen::MatrixXcf channelMatH = channelMat.adjoint();

  Eigen::MatrixXcf precodeMat =
      channelMatH * (channelMat * channelMatH).inverse();

  return precodeMat;
}

Eigen::MatrixXcf calculateZFSVD(const Eigen::MatrixXcf &H) {

  Eigen::JacobiSVD<Eigen::MatrixXcf> svd(H, Eigen::ComputeThinU |
                                                Eigen::ComputeThinV);

  return svd.solve(Eigen::MatrixXcf::Identity(H.rows(), H.rows()));
}

std::vector<std::vector<Antenna>>
applyPrecode(const Eigen::MatrixXcf &precodeMat,
             const std::vector<Antenna> &antennaVec) {

  // We return one antenna vector per User

  std::vector<std::vector<Antenna>> result;
  std::vector<Antenna> temp;

  for (int j = 0; j < precodeMat.cols(); j++) { // One Column = One User

    std::vector<Antenna> temp;
    for (int i = 0; i < precodeMat.rows(); i++) { // One Row = One Antenna

      temp.push_back(Antenna(antennaVec[i].getPosition(), precodeMat(i, j)));
    }

    result.push_back(temp);
  }
  return result;
}

} // namespace INF8503::model
