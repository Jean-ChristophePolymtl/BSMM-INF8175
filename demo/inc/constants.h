#pragma once

#include <raylib.h>
#include <string>

namespace INF8503::constants {
constexpr auto LIGHT_SPEED{299792458.0}; // [m/s]

constexpr int SCREEN_HEIGHT{600};
constexpr int SCREEN_WIDTH{800};
constexpr int TARGET_FPS{60};
const std::string WINDOW_NAME{"MIMO Visualizer"};
constexpr Vector2 CENTER{(float)SCREEN_WIDTH / 2.0, (float)SCREEN_HEIGHT / 2.0};

} // namespace INF8503::constants