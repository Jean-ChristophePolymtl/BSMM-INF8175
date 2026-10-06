#pragma once

#include "model/antenna.h"
#include <iostream>
#include <vector>

namespace INF8503::model {

class AntennaArray {

public:
  AntennaArray();
  ~AntennaArray();

  std::vector<Antenna> getAntennaArray() const;

  void createUniformLinearArray(float x, float y, int num, float spacing);
  void addAntenna(float x, float y, float phase, float amplitude,
                  float frequency);

private:
  std::vector<Antenna> _antennaArray;
};

} // namespace INF8503::model