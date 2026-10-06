#include "render/renderer.h"

namespace INF8503::render {

constexpr float ANTENNA_RADIUS{10.0};

void draw() {}

void drawAntennaArray(const model::AntennaArray array) {
  std::vector<model::Antenna> vec = array.getAntennaArray();
  for (auto a : vec) {
    drawAntenna(a);
  }
}

void drawAntenna(const model::Antenna a) {
  DrawCircle(a.getX(), a.getY(), ANTENNA_RADIUS, RED);
}

void drawUser() {}
void drawRadiationPattern() {}

} // namespace INF8503::render