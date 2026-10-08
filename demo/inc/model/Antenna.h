#pragma once

#include <Eigen/Dense>
#include <complex>

namespace INF8503::model {

class Antenna {

public:
  Antenna();
  Antenna(float x, float y, float phase, float amplitude, float frequency);
  ~Antenna();

  float getX() const;
  float getY() const;
  float getPhase() const;
  float getFrequency() const;
  float getAmplitude() const;
  std::complex<float> getPhasor() const;
  Eigen::Vector2f getPosition() const;

  void setPosition(float x, float y);
  void setX(float x);
  void setY(float y);
  void setPhase(float phase);
  void setFrequency(float frequency);
  void setAmplitude(float amplitude);
  void setPhasor(std::complex<float> phasor);
  void setPosition(Eigen::Vector2f position);

private:
  float _x, _y;

  Eigen::Vector2f _position;
  std::complex<float> _phasor;

  float _phase;
  float _frequency;
  float _amplitude;
};

} // namespace INF8503::model