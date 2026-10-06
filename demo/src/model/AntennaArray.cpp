#include "model/AntennaArray.h"

namespace INF8503::model {

AntennaArray::AntennaArray() {}
AntennaArray::~AntennaArray() {}

float AntennaArray::getX() const { return this->_x; }
float AntennaArray::getY() const { return this->_y; }

void AntennaArray::setX(float x) { this->_x = x; }
void AntennaArray::setY(float y) { this->_y = y; }

std::vector<Antenna> AntennaArray::getAntennaArray() const {
  return this->_antennaArray;
}

void AntennaArray::createUniformLinearArray(float x, float y, int num,
                                            float spacing) {
  if (!_antennaArray.empty()) {
    std::cout << "This antenna array object already initialized";
    return;
  }

  for (auto i{0}; i < num; i++) {
    addAntenna(x - (spacing * num / 2) + (i * spacing), y, 0.0, 1.0,
               1000.0); // TODO changer les valeurs hard coded
  }
}

void AntennaArray::addAntenna(float x, float y, float phase, float amplitude,
                              float frequency) {

  this->_antennaArray.push_back(Antenna(x, y, phase, amplitude, frequency));
}
} // namespace INF8503::model