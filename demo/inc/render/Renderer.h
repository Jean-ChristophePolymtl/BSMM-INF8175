#pragma once

#include "constants.h"
#include "math/Math.h"
#include "model/AntennaArray.h"
#include "model/User.h"
#include "render/Shader.h"
#include <iostream>
#include <raylib.h>

namespace INF8503::render {

void draw();
void drawAntennaArray(const model::AntennaArray &array);
void drawAntenna(const model::Antenna &antenna);
void drawUsers(const std::vector<model::User> &users, float scale);
void drawUser(const model::User &user, float scale);
void drawRadiationPattern();
void drawPowerPatternDB(const std::vector<float> &power, float scale);

} // namespace INF8503::render