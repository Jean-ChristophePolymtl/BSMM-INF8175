#pragma once

#include "constants.h"
#include "math/Math.h"
#include "model/AntennaArray.h"
#include "render/Shader.h"
#include <iostream>
#include <raylib.h>

namespace INF8503::render {

void draw();
void drawAntennaArray(const model::AntennaArray);
void drawAntenna(const model::Antenna a);
void drawUser();
void drawRadiationPattern();
void drawPowerPatternDB(const std::vector<float> power, float scale);

} // namespace INF8503::render