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

void drawPowerPatternDB(const std::vector<float> power, float scale) {

  std::vector<Vector2> plot;
  Vector2 center = constants::CENTER;

  for (auto i{0}; i < 360; i++) {
    Color color = colorGradient(power[i], -50.0f, 0.0f);
    float t = Clamp(Remap(power[i], -50.0f, 0.0f, 0.0f, 1.0f * scale), 0.0f,
                    1.0f * scale);
    Vector2 point = math::polarToCartesian(t, (float)i, center);
    plot.push_back(point);
    // DrawLineV(center, point, color);
  }
  DrawLineStrip(plot.data(), plot.size(), BLUE);
}

void drawUser() {}
void drawRadiationPattern() {}

} // namespace INF8503::render