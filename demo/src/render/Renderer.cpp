#include "render/Renderer.h"

namespace INF8503::render {

constexpr float ANTENNA_RADIUS{0.1};

void draw() {}

void drawAntennaArray(const model::AntennaArray array) {

  std::vector<model::Antenna> vec = array.getAntennaArray();
  for (auto a : vec) {
    drawAntenna(a);
  }
}

void drawAntenna(const model::Antenna a) {
  DrawCircle(a.getX(), a.getY(), ANTENNA_RADIUS, GREEN);
}

void drawPowerPatternDB(const std::vector<float> power) {
  for (auto i{0}; i < 360; i++) {
    Color color = colorGradient(power[i], -40.0f, 0.0f);
    Vector2 center = constants::CENTER;
    Vector2 end = math::polarToCartesian(500.0f, (float)i, center);
    DrawLineV(center, end, color);
  }
}

void drawUser() {}
void drawRadiationPattern() {}

} // namespace INF8503::render