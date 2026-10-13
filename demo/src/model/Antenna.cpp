#include "model/Antenna.h"

namespace INF8503::model {

Antenna::Antenna()
    : _x{0.0f}, _y{0.0f}, _phase{0.0f}, _frequency{0.0f}, _amplitude{0.0f} {}

Antenna::Antenna(float x, float y, float amplitude, float phase,
                 float frequency)
    : _x{x}, _y{y}, _phase{phase}, _frequency{frequency}, _amplitude{amplitude},
      _position{x, y}, _phasor{std::polar(amplitude, phase)} {}

Antenna::Antenna(const Eigen::Vector2f position,
                 const std::complex<float> phasor)
    : _position{position}, _phasor{phasor} {}

Antenna::Antenna(float x, float y, const std::complex<float> _phasor) {}

Antenna::~Antenna() {}

float Antenna::getX() const { return _x; }

float Antenna::getY() const { return _y; }

float Antenna::getPhase() const { return _phase; }

float Antenna::getFrequency() const { return _frequency; }

float Antenna::getAmplitude() const { return _amplitude; }

std::complex<float> Antenna::getPhasor() const { return _phasor; }

Eigen::Vector2f Antenna::getPosition() const { return _position; }

void Antenna::setPosition(float x, float y) {
  setX(x);
  setY(y);
}

void Antenna::setX(float x) { _x = x; }

void Antenna::setY(float y) { _y = y; }

void Antenna::setPhase(float phase) { _phase = phase; }

void Antenna::setFrequency(float frequency) { _frequency = frequency; }

void Antenna::setAmplitude(float amplitude) { _amplitude = amplitude; }

void Antenna::setPhasor(const std::complex<float> &phasor) { _phasor = phasor; }

void Antenna::setPosition(const Eigen::Vector2f &position) {
  _position = position;
}

} // namespace INF8503::model