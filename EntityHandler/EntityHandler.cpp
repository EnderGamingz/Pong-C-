#include "../Ball/Ball.h"
#include "../PointCounter/PointCounter.h"
#include "EntityHandler.h"

#include <iostream>

void EntityHandler::init_players() {
  // Initialize players
  cout << "EntityHandler: Initializing players" << endl;
  vector<Player*> _players;

  _players.push_back(new Player(PlayerSide::LEFT, Keyboard::W, Keyboard::S));
  _players.push_back(new Player(PlayerSide::RIGHT, Keyboard::Up, Keyboard::Down));

  this->players = _players;

  cout << "EntityHandler: Players initialized" << endl;
}

void EntityHandler::init_ball() {
  cout << "EntityHandler: Initializing ball" << endl;
  // Initialize ball
  this->ball = new Ball();

  cout << "EntityHandler: Ball initialized" << endl;
}

void EntityHandler::init_pointCounter() {
  cout << "EntityHandler: Initializing pointCounter" << endl;
  // Initialize point counter
  this->pointCounter = new PointCounter();

  cout << "EntityHandler: PointCounter initialized" << endl;
}

EntityHandler &EntityHandler::getInstance() {
  static EntityHandler instance;
  return instance;
}

vector<Player*> *EntityHandler::getPlayers() {
  return &players;
}

Ball *EntityHandler::getBall() {
  return ball;
}

PointCounter *EntityHandler::getPointCounter() {
  return pointCounter;
}

int EntityHandler::getCurrentBallOwnerIndex() const {
  return currentBallOwnerIndex;
}

void EntityHandler::setBallOwnerIndex(int index) {
  currentBallOwnerIndex = index;
}

void EntityHandler::reset() {
  currentBallOwnerIndex = -1;
  ball->reset();
  for (auto &player: players) {
    player->reset();
  }
}

vector<PowerUp *> *EntityHandler::getPowerUps() {
  return &powerUps;
}

void EntityHandler::addPowerUp(PowerUp *powerUp) {
  powerUps.push_back(powerUp);
}

Vector2f EntityHandler::getBallPosition() {
  return ball->getBody()->getPosition();
}

void EntityHandler::prepareOnlineGame() {

}

EntityHandler::EntityHandler() = default;