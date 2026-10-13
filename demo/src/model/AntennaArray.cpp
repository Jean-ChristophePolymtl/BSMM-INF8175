#include "model/AntennaArray.h"

namespace INF8503::model {

AntennaArray::AntennaArray() {}
AntennaArray::~AntennaArray() {}

float AntennaArray::getX() const { return _x; }
float AntennaArray::getY() const { return _y; }
float AntennaArray::getFrequency() const { return _frequency; }
Eigen::Vector2f AntennaArray::getPosition() const { return _position; }

void AntennaArray::setX(float x) { _x = x; }
void AntennaArray::setY(float y) { _y = y; }
void AntennaArray::setPosition(const Eigen::Vector2f &pos) { _position = pos; }

std::vector<Antenna> AntennaArray::getAntennaArray() const {
  return _antennaArray;
}

void AntennaArray::createUniformLinearArray(float x, float y, int num,
                                            float spacing) {
  if (!_antennaArray.empty()) {
    std::cout << "This antenna array object already initialized";
    return;
  }

  for (auto i{0}; i < num; i++) {
    float antennaX = x + (i - (num - 1) / 2.0f) * spacing;
    addAntenna(antennaX, y, 1.0, 0.0,
               _frequency); // TODO changer les valeurs hard coded
  }
}

void AntennaArray::createUniformRectangularArray(float x, float y, int rows,
                                                 int cols, float spacing) {
  if (!_antennaArray.empty()) {
    std::cout << "This antenna array object already initialized";
    return;
  }

  for (auto row{0}; row < rows; row++) {
    for (auto col{0}; col < cols; col++) {

      float antennaX = x + (col - (cols - 1) / 2.0f) * spacing;
      float antennaY = y + (row - (rows - 1) / 2.0f) * spacing;

      addAntenna(antennaX, antennaY, 1.0f, 0.0f,
                 _frequency); // TODO changer les valeurs hard coded
    }
  }
}

void AntennaArray::addAntenna(float x, float y, float amplitude, float phase,
                              float frequency) {

  _antennaArray.push_back(Antenna(x, y, amplitude, phase, frequency));
}
} // namespace INF8503::model