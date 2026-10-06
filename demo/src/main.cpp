#include "model/antennaArray.h"
#include "render/renderer.h"
#include <raylib.h>
#include <string>

constexpr int SCREEN_HEIGHT{600};
constexpr int SCREEN_WIDTH{800};
constexpr int TARGET_FPS{60};
const std::string WINDOW_NAME{"MIMO Visualizer"};
constexpr Vector2 CENTER{(float)SCREEN_WIDTH / 2.0, (float)SCREEN_HEIGHT / 2.0};

INF8503::model::AntennaArray array;

int main() {

  array.createUniformLinearArray(CENTER.x, CENTER.y, 4, 50.0);

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_NAME.c_str());

  SetTargetFPS(TARGET_FPS);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);
    INF8503::render::drawAntennaArray(array);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}