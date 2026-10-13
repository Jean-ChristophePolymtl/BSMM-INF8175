#pragma once

#include "constants.h"
#include "math/Math.h"
#include <Eigen/Dense>
#include <raylib.h>

namespace INF8503::model {

class User {
public:
  User();
  ~User();

  void setPosition(const Eigen::Vector2f &position);
  void setPositionPolar(float radius, float angle);

  Eigen::Vector2f getPosition() const;
  float getX() const;
  float getY() const;
  float getAmplitude() const;
  float getAngle() const;

private:
  float _x, _y, _angle, _amplitude;
  Eigen::Vector2f _position;
};

} // namespace INF8503::model