#include "Application.h"

namespace INF8503::app {

INF8503::model::AntennaArray array;
std::vector<std::vector<INF8503::model::Antenna>> phasedArray;
std::vector<INF8503::model::User> users;
float scale{200.0f};

Vector2 characterController(const Vector2 &userPos) {

  Vector2 direction = {0.0f, 0.0f};
  const float playerSpeed{150.0f};

  if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    direction.y -= 1.0f;

  if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    direction.y += 1.0f;

  if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    direction.x -= 1.0f;

  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    direction.x += 1.0f;

  // Normalize direction to prevent faster diagonal movement
  if (Vector2Length(direction) > 0.0f)
    direction = Vector2Normalize(direction);

  // Update player position
  float dt = GetFrameTime();

  Vector2 newPosi;

  newPosi.x = userPos.x + (direction.x * playerSpeed * dt);
  newPosi.y = userPos.y + (direction.y * playerSpeed * dt);

  return newPosi;
}

void run() {

  array.createUniformRectangularArray(INF8503::constants::CENTER.x,
                                      INF8503::constants::CENTER.y, 4, 4,
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

  Eigen::MatrixXcf conjPrecodingMat = precodingMat.conjugate().reverse();

  phasedArray =
      INF8503::model::applyPrecode(conjPrecodingMat, array.getAntennaArray());

  std::vector<float> power = INF8503::sim::calculatePower(phasedArray[3]);

  InitWindow(INF8503::constants::SCREEN_WIDTH,
             INF8503::constants::SCREEN_HEIGHT,
             INF8503::constants::WINDOW_NAME.c_str());

  SetTargetFPS(INF8503::constants::TARGET_FPS);

  while (!WindowShouldClose()) {

    Vector2 userPos;
    userPos.x = users[0].getX();
    userPos.y = users[0].getY();

    userPos = characterController(userPos);

    Eigen::Vector2f newPos = {userPos.x, userPos.y};

    users[0].setPosition(newPos);

    channelMat = INF8503::model::calculateChannelMatrix(array, users);

    precodingMat = INF8503::model::calculateZFSVD(channelMat);

    Eigen::MatrixXcf conjPrecodingMat = precodingMat.conjugate();

    phasedArray =
        INF8503::model::applyPrecode(conjPrecodingMat, array.getAntennaArray());

    std::vector<float> power = INF8503::sim::calculatePower(phasedArray[0]);

    BeginDrawing();

    ClearBackground(RAYWHITE);

    INF8503::render::drawPowerPatternDB(INF8503::math::powerVectorToDB(power),
                                        scale);
    INF8503::render::drawAntennaArray(array);

    INF8503::render::drawUser(users[0], scale);

    EndDrawing();
  }

  CloseWindow();
}
} // namespace INF8503::app