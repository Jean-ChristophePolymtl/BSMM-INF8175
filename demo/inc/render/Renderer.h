#pragma once

#include "model/AntennaArray.h"
#include <raylib.h>

namespace INF8503::render {

void draw();
void drawAntennaArray(const model::AntennaArray);
void drawAntenna(const model::Antenna a);
void drawUser();
void drawRadiationPattern();

} // namespace INF8503::render