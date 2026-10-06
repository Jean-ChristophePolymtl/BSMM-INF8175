#pragma once

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

  void setPosition(float x, float y);
  void setX(float x);
  void setY(float y);
  void setPhase(float phase);
  void setFrequency(float frequency);
  void setAmplitude(float amplitude);

private:
  float _x, _y;

  float _phase;
  float _frequency;
  float _amplitude;
};

} // namespace INF8503::model