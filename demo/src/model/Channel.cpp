#include "model/Channel.h"

namespace INF8503::model {

std::complex<float> freeSpaceLOS(float lambda, float distance) {
  return std::polar(lambda / (4 * PI * distance),
                    (-2 * PI * distance) / lambda);
}

Eigen::MatrixXcf calculateChannelMatrix(const AntennaArray &antennaArray,
                                        const std::vector<User> &usersVec) {

  std::vector<User> users = usersVec;
  std::vector<Antenna> antennas = antennaArray.getAntennaArray();
  Eigen::MatrixXcf channelMat = Eigen::MatrixXcf(users.size(), antennas.size());

  int i{0};
  for (auto user : users) {
    int j{0};
    for (auto antenna : antennas) {
      float distance =
          math::distance(user.getPosition(), antenna.getPosition());
      channelMat(i, j) = freeSpaceLOS(constants::LAMBDA, distance);
      j++;
    }
    i++;
  }

  return channelMat;
}

} // namespace INF8503::model
