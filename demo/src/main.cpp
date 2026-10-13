#include "constants.h"
#include "math/Math.h"
#include "render/Renderer.h"
#include "simulation/RadiationCalculator.h"
#include "simulation/Simulation.h"
#include <Eigen/Dense>
#include <algorithm>
#include <cmath>
#include <raylib.h>
#include <string>

INF8503::model::AntennaArray array;
std::vector<std::vector<INF8503::model::Antenna>> phasedArray;
std::vector<INF8503::model::User> users;
float scale{200.0f};

int main() {

  array.createUniformRectangularArray(INF8503::constants::CENTER.x,
                                      INF8503::constants::CENTER.y, 8, 8,
                                      INF8503::constants::LAMBDA / 2.0);

  INF8503::model::User user1;
  INF8503::model::User user2;
  INF8503::model::User user3;
  INF8503::model::User user4;

  user1.setPositionPolar(20.0f, 20.0f);
  user2.setPositionPolar(20.0f, 60.0f);
  user3.setPositionPolar(20.0f, 120.0f);
  user4.setPositionPolar(20.0f, 160.0f);

  std::vector<INF8503::model::User> users;

  users.push_back(user1);
  users.push_back(user2);
  users.push_back(user3);
  users.push_back(user4);

  Eigen::MatrixXcf channelMat =
      Eigen::MatrixXcf(users.size(), array.getAntennaArray().size());

  Eigen::MatrixXcf precodingMat =
      Eigen::MatrixXcf(array.getAntennaArray().size(), users.size());

  // simulation update()

  channelMat = INF8503::model::calculateChannelMatrix(array, users);
  precodingMat = INF8503::model::calculateZFSVD(channelMat);
  phasedArray =
      INF8503::model::applyPrecode(precodingMat, array.getAntennaArray());

  std::cout << "H:\n" << channelMat << "\n\n";
  std::cout << "W:\n" << precodingMat << "\n\n";
  std::cout << "H * W:\n" << channelMat * precodingMat << "\n\n";

  std::vector<float> power = INF8503::sim::calculatePower(phasedArray[0]);

  InitWindow(INF8503::constants::SCREEN_WIDTH,
             INF8503::constants::SCREEN_HEIGHT,
             INF8503::constants::WINDOW_NAME.c_str());

  SetTargetFPS(INF8503::constants::TARGET_FPS);

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    INF8503::render::drawPowerPatternDB(INF8503::math::powerVectorToDB(power),
                                        scale);
    INF8503::render::drawAntennaArray(array);
    INF8503::render::drawUsers(users, 10.0f);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}