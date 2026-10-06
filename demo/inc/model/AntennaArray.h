#pragma once

#include "model/Antenna.h"
#include <iostream>
#include <vector>

namespace INF8503::model {

class AntennaArray {

public:
  AntennaArray();
  ~AntennaArray();

  std::vector<Antenna> getAntennaArray() const;
  float getX() const;
  float getY() const;

  void setX(float x);
  void setY(float y);

  void createUniformLinearArray(float x, float y, int num, float spacing);
  void addAntenna(float x, float y, float phase, float amplitude,
                  float frequency);

private:
  float _x, _y;
  std::vector<Antenna> _antennaArray;
};

} // namespace INF8503::model