#include "simulation/Simulation.h"

namespace INF8503::sim {

Simulation::Simulation() {}
Simulation::~Simulation() {}

void Simulation::setAntennaArray(const model::AntennaArray &array) {
  this->_antennaArray = array;
}
void Simulation::setUsers(const std::vector<model::User> &users) {
  this->_users = users;
}

void Simulation::setNumUsers(int n) { this->_numUsers = n; }

void Simulation::setNumAntennas(int n) { this->_numAntennas = n; }

model::AntennaArray Simulation::getAntennaArray() const {
  return this->_antennaArray;
}
std::vector<model::User> Simulation::getUsers() const { return this->_users; }

int Simulation::getNumUsers() const { return this->_numUsers; }

int Simulation::getNumAntennas() const { return this->_numAntennas; }

void Simulation::initialize(const model::AntennaArray &array,
                            const std::vector<model::User> &users) {
  setAntennaArray(array);
  setUsers(users);

  setNumAntennas(array.getAntennaArray().size());
  setNumUsers(users.size());

  this->_channelMatrix = Eigen::MatrixXcf(getNumUsers(), getNumAntennas());

  this->_precodingMatrix = Eigen::MatrixXcf(getNumAntennas(), getNumUsers());
}

void Simulation::update() {}
void Simulation::updateChannel() {}
void Simulation::updatePrecoding() {}
void Simulation::updateAntennaArray() {}
void Simulation::updateUsers() {}



} // namespace INF8503::sim
