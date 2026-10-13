#include "render/Renderer.h"

namespace INF8503::render {

constexpr float ANTENNA_RADIUS{0.1};
constexpr float USER_RADIUS{5.0};

void draw() {}

void drawAntennaArray(const model::AntennaArray &array) {

  std::vector<model::Antenna> vec = array.getAntennaArray();
  for (auto a : vec) {
    drawAntenna(a);
  }
}

void drawAntenna(const model::Antenna &antenna) {
  DrawCircle(antenna.getX(), antenna.getY(), ANTENNA_RADIUS, GREEN);
}

void drawUsers(const std::vector<model::User> &users, float scale = 1.0f) {

  for (auto u : users) {
    drawUser(u, scale);
  }
}

void drawUser(const model::User &user, float scale = 1.0f) {

  Vector2 position = math::polarToCartesian(user.getAmplitude() * scale,
                                            user.getAngle(), constants::CENTER);

  DrawCircle(position.x, position.y, USER_RADIUS, RED);
}

void drawPowerPatternDB(const std::vector<float> &power, float scale) {

  std::vector<Vector2> plot;
  Vector2 center = constants::CENTER;

  for (auto i{0}; i < constants::N_ANGLES; i++) {

    float powerToScale = Clamp(
        Remap(power[i], constants::MIN_DB, constants::MAX_DB, 0.0f, scale),
        0.0f, scale);
    Vector2 point = math::polarToCartesian(powerToScale, (float)i, center);
    plot.push_back(point);
  }

  DrawLineStrip(plot.data(), plot.size(), BLUE);
}

void drawRadiationPattern() {}

} // namespace INF8503::render