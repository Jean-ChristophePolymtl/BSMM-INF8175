#include "model/antenna.h"

namespace INF8503::model {

Antenna::Antenna()
    : _x{0.0f}, _y{0.0f}, _phase{0.0f}, _frequency{0.0f}, _amplitude{0.0f} {}

Antenna::Antenna(float x, float y, float phase, float amplitude,
                 float frequency)
    : _x{x}, _y{y}, _phase{phase}, _frequency{frequency},
      _amplitude{amplitude} {}

Antenna::~Antenna() {}

float Antenna::getX() const { return this->_x; }

float Antenna::getY() const { return this->_y; }

float Antenna::getPhase() const { return this->_phase; }

float Antenna::getFrequency() const { return this->_frequency; }

float Antenna::getAmplitude() const { return this->_amplitude; }

void Antenna::setPosition(float x, float y) {
  setX(x);
  setY(y);
}

void Antenna::setX(float x) { this->_x = x; }

void Antenna::setY(float y) { this->_y = y; }

void Antenna::setPhase(float phase) { this->_phase = phase; }

void Antenna::setFrequency(float frequency) { this->_frequency = frequency; }

void Antenna::setAmplitude(float amplitude) { this->_amplitude = amplitude; }
} // namespace INF8503::model