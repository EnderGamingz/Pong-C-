#ifndef PONG_ENTITYHANDLER_H
#define PONG_ENTITYHANDLER_H
#include "SFML/Graphics.hpp"

class Ball;
class PointCounter;
class Player;
class PowerUp;

using namespace sf;
using namespace std;

class EntityHandler {
public:
  EntityHandler();
  static EntityHandler &getInstance();

  vector<Player*> *getPlayers();
  Ball *getBall();
  Vector2f getBallPosition();
  PointCounter *getPointCounter();

  vector<PowerUp*> *getPowerUps();
  void addPowerUp(PowerUp *powerUp);

  void init_players();
  void init_ball();
  void init_pointCounter();

  void prepareOnlineGame();

  [[nodiscard]] int getCurrentBallOwnerIndex() const;
  void setBallOwnerIndex(int index);

  void reset();

private:
  vector<Player*> players;
  Ball *ball;
  PointCounter *pointCounter;
  int currentBallOwnerIndex = -1;
  vector<PowerUp*> powerUps;

public:
  // Disable copy and assign
  EntityHandler(const EntityHandler &) = delete;
  void operator=(const EntityHandler &) = delete;
};

#endif//PONG_ENTITYHANDLER_H