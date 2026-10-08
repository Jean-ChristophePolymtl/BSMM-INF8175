#include "constants.h"
#include "math/Math.h"
#include "model/AntennaArray.h"
#include "render/Renderer.h"
#include "simulation/RadiationCalculator.h"
#include <Eigen/Dense>
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <string>

INF8503::model::AntennaArray array;
INF8503::sim::RadiationCalculator calc;
float scale{300.0f};

int main() {

  array.createUniformRectangularArray(
      INF8503::constants::CENTER.x, INF8503::constants::CENTER.y, 4, 4, 0.05f);

  InitWindow(INF8503::constants::SCREEN_WIDTH,
             INF8503::constants::SCREEN_HEIGHT,
             INF8503::constants::WINDOW_NAME.c_str());

  SetTargetFPS(INF8503::constants::TARGET_FPS);

  std::vector<float> power = calc.calculatePower(array);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    INF8503::render::drawPowerPatternDB(INF8503::math::powerVectorToDB(power),
                                        scale);
    INF8503::render::drawAntennaArray(array);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}