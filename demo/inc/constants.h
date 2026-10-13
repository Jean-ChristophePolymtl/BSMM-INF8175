#pragma once

#include <Eigen/Dense>
#include <raylib.h>
#include <string>
#include <vector>

namespace INF8503::constants {
constexpr auto LIGHT_SPEED{299792458.0}; // [m/s]
constexpr auto FREQUENCY{3e9};
constexpr auto LAMBDA{LIGHT_SPEED / FREQUENCY};

constexpr auto SCREEN_HEIGHT{600};
constexpr auto SCREEN_WIDTH{800};
constexpr auto TARGET_FPS{60};
const std::string WINDOW_NAME{"MIMO Visualizer"};
constexpr Vector2 CENTER{(float)SCREEN_WIDTH / 2.0, (float)SCREEN_HEIGHT / 2.0};
const Eigen::Vector2f CENTER_EIGEN{(float)SCREEN_WIDTH / 2.0,
                                   (float)SCREEN_HEIGHT / 2.0};

constexpr auto MAX_DB{0.0f};
constexpr auto MIN_DB{-50.0f};

constexpr auto N_ANGLES{180};

const std::vector<float> ANGLES_RAD = [] {
  std::vector<float> result(N_ANGLES);

  for (int i = 0; i < N_ANGLES; ++i) {
    result[i] = PI * i / 180.0f;
  }

  return result;
}();

} // namespace INF8503::constants