#include "render/Shader.h"

namespace INF8503::render {

Color colorGradient(float input, float min, float max) {
  float t = Clamp(Remap(input, min, max, 0.0f, 1.0f), 0.0f, 1.0f);

  if (t < 0.5f) {
    return ColorLerp(BLUE, GREEN, t);
  } else {
    return ColorLerp(GREEN, RED, t);
  }
  return BLUE;
}

} // namespace INF8503::render