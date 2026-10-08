#include "simulation/RadiationCalculator.h"

namespace INF8503::sim {

RadiationCalculator::RadiationCalculator() {
  for (auto i{0.0f}; i < 360.0f; i += 1.0f) {
    _angle.push_back(i);
  }
}
RadiationCalculator::~RadiationCalculator() {}

void RadiationCalculator::setPower() {}
void RadiationCalculator::setAngle() {}

std::vector<float> RadiationCalculator::getPower() const {
  return this->_power;
}

std::vector<float> RadiationCalculator::getAngle() const {
  return this->_angle;
}

std::vector<float>
RadiationCalculator::calculatePower(const model::AntennaArray array) const {

  float k = (2 * PI * array.getFrequency()) / constants::LIGHT_SPEED;

  std::vector<model::Antenna> vec = array.getAntennaArray();
  std::vector<float> power;

  for (auto angle : this->_angle) {

    const float angleRad = angle * DEG2RAD;
    std::complex<float> sum;

    for (auto ant : vec) {
      Eigen::Vector2f direction{std::cos(angleRad), std::sin(angleRad)};
      Eigen::Vector2f distance =
          math::distanceVector(ant.getPosition(), array.getPosition());

      float theta = k * distance.dot(direction);

      std::complex<float> delta =
          /*std::polar(ant.getAmplitude(), ant.getPhase()) */
          std::polar(1.0f, theta);

      sum += delta;
    }

    power.push_back(std::norm(sum));
  }

  return power;
}

} // namespace INF8503::sim