#pragma once

#include "constants.h"
#include "model/AntennaArray.h"
#include <Eigen/Dense>
#include <vector>

namespace INF8503::sim {

class RadiationCalculator {
public:
  RadiationCalculator();
  ~RadiationCalculator();

  void setPower();
  void setAngle();

  std::vector<float> getPower() const;
  std::vector<float> getAngle() const;

  std::vector<float> calculatePower(const model::AntennaArray array) const;

private:
  std::vector<float> _power;
  std::vector<float> _angle;
};

} // namespace INF8503::sim