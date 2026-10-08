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
  float getFrequency() const;
  Eigen::Vector2f getPosition() const;

  void setX(float x);
  void setY(float y);
  void setPosition(Eigen::Vector2f pos);

  void createUniformLinearArray(float x, float y, int num, float spacing);

  void createUniformRectangularArray(float x, float y, int rows, int cols,
                                     float spacing);

  void addAntenna(float x, float y, float phase, float amplitude,
                  float frequency);

private:
  float _x, _y;
  float _frequency{3e9};

  Eigen::Vector2f _position;

  std::vector<Antenna> _antennaArray;
};

} // namespace INF8503::model