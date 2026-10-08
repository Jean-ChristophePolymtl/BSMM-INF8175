#include "model/AntennaArray.h"

namespace INF8503::model {

AntennaArray::AntennaArray() {}
AntennaArray::~AntennaArray() {}

float AntennaArray::getX() const { return this->_x; }
float AntennaArray::getY() const { return this->_y; }
float AntennaArray::getFrequency() const { return this->_frequency; }
Eigen::Vector2f AntennaArray::getPosition() const { return this->_position; }

void AntennaArray::setX(float x) { this->_x = x; }
void AntennaArray::setY(float y) { this->_y = y; }
void AntennaArray::setPosition(Eigen::Vector2f pos) { this->_position = pos; }

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
    float antennaX = x + (i - (num - 1) / 2.0f) * spacing;
    addAntenna(antennaX, y, 0.0, 1.0,
               getFrequency()); // TODO changer les valeurs hard coded
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

      addAntenna(antennaX, antennaY, 0.0f, 1.0f,
                 getFrequency()); // TODO changer les valeurs hard coded
    }
  }
}

void AntennaArray::addAntenna(float x, float y, float phase, float amplitude,
                              float frequency) {

  this->_antennaArray.push_back(Antenna(x, y, phase, amplitude, frequency));
}
} // namespace INF8503::model