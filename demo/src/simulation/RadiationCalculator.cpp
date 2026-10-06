#include "simulation/RadiationCalculator.h"
#include <vector>

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

  float k = 2 * constants::PI / constants::LIGHT_SPEED;
  std::vector<float> vec;

  return vec;
}

} // namespace INF8503::sim