#pragma once

#include "model/AntennaArray.h"
#include "model/Channel.h"
#include "model/Precoder.h"
#include "model/User.h"

namespace INF8503::sim {

class Simulation {

public:
  Simulation();
  ~Simulation();

  void setAntennaArray(const model::AntennaArray &array);
  void setUsers(const std::vector<model::User> &users);
  void setNumUsers(int n);
  void setNumAntennas(int n);

  model::AntennaArray getAntennaArray() const;
  std::vector<model::User> getUsers() const;
  int getNumUsers() const;
  int getNumAntennas() const;

  void initialize(const model::AntennaArray &array,
                  const std::vector<model::User> &users);
  void update();
  void updateChannel();
  void updatePrecoding();
  void updateAntennaArray();
  void updateUsers();

private:
  int _numUsers, _numAntennas;
  Eigen::MatrixXcf _channelMatrix;
  Eigen::MatrixXcf _precodingMatrix;
  model::AntennaArray _antennaArray;
  std::vector<model::User> _users;
};

} // namespace INF8503::sim
