#include "model/User.h"

namespace INF8503::model {

User::User() {}
User::~User() {}

void User::setPosition(const Eigen::Vector2f &position) {
  _position = position;
}
void User::setPositionPolar(const float radius, const float angle) {
  Vector2 position = math::polarToCartesian(radius, angle, constants::CENTER);
  _position = Eigen::Vector2f{position.x, position.y};
  _angle = angle;
  _amplitude = radius;
}

float User::getAmplitude() const { return _amplitude; }
float User::getAngle() const { return _angle; }

float User::getX() const { return _position[0]; }
float User::getY() const { return _position[1]; }

Eigen::Vector2f User::getPosition() const { return _position; }

} // namespace INF8503::model