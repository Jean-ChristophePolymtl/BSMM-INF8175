#include "simulation/RadiationCalculator.h"

namespace INF8503::sim {

std::vector<float> calculatePower(const model::AntennaArray &antennaArray) {

  float k = (2 * PI * antennaArray.getFrequency()) / constants::LIGHT_SPEED;
  std::vector<model::Antenna> array = antennaArray.getAntennaArray();
  std::vector<float> power;

  for (auto angle : constants::ANGLES_RAD) {

    std::complex<float> sum;

    for (auto antenna : array) {
      Eigen::Vector2f direction{std::cos(angle), std::sin(angle)};
      Eigen::Vector2f distance = math::distanceVector(
          antenna.getPosition(), antennaArray.getPosition());

      float theta = k * distance.dot(direction);

      std::complex<float> delta = antenna.getPhasor() * std::polar(1.0f, theta);

      sum += delta;
    }

    power.push_back(std::norm(sum));
  }

  return power;
}

std::vector<float>
calculatePower(const std::vector<model::Antenna> &antennaVec) {

  float k = (2 * PI * constants::FREQUENCY) / constants::LIGHT_SPEED;
  std::vector<float> power;

  for (auto angle : constants::ANGLES_RAD) {

    std::complex<float> sum;

    for (auto antenna : antennaVec) {
      Eigen::Vector2f direction{std::cos(angle), std::sin(angle)};
      Eigen::Vector2f distance =
          math::distanceVector(antenna.getPosition(), constants::CENTER_EIGEN);

      float theta = k * distance.dot(direction);

      std::complex<float> delta = antenna.getPhasor() * std::polar(1.0f, theta);

      sum += delta;
    }

    power.push_back(std::norm(sum));
  }

  return power;
}
} // namespace INF8503::sim