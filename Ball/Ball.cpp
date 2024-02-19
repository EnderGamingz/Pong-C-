#include "Ball.h"
#include "../PointCounter/PointCounter.h"
#include "../SoundHandler/SoundHandler.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-integer-division"
Ball::Ball() {
  cout << "Ball: initializing" << endl;
  this->window = GameHandler::getInstance().getWindow();
  cout << "Ball: Constructor got window" << endl;
  this->players = EntityHandler::getInstance().getPlayers();
  cout << "Ball: Constructor got players" << endl;
  this->powerUps = EntityHandler::getInstance().getPowerUps();
  cout << "Ball: Constructor got power ups" << endl;

  this->pos_y = static_cast<float>(this->window->getSize().y / 2);
  this->pos_x = static_cast<float>(this->window->getSize().x / 2);
  CircleShape _body;
  _body.setRadius(radius);
  _body.setPosition(pos_x, pos_y);
  _body.setOrigin(radius, radius);
  _body.setFillColor(Color::White);

  cout << "Ball: Constructor created body" << endl;

  mt19937 &engine = RandomEngine::getInstance().getEngine();
  uniform_int_distribution<> startingAngle(0, 360);

  cout << "Ball: Constructor got random engine" << endl;

  this->angle = static_cast<float>(startingAngle(engine));

  this->body = _body;

  cout << "Ball: initialized" << endl;
}
#pragma clang diagnostic pop

void Ball::update() {
  auto currentOwner = EntityHandler::getInstance().getCurrentBallOwnerIndex();
  bool ballIsOwned = currentOwner != -1 && currentOwner <= players->size();

  // Check for collisions
  if (pos_y >= static_cast<float>(window->getSize().y) - radius || pos_y <= radius) {
    // Flip the angle if collision is on the top or bottom
    angle = -angle;
    SoundHandler::getInstance().playSound(GameSounds[BOUNCE_WALL]);
  }

  if (pos_x >= static_cast<float>(window->getSize().x) - radius || pos_x <= radius) {

    if (ballIsOwned) {
      EntityHandler::getInstance().getPointCounter()->increment(currentOwner);
    }

    EntityHandler::getInstance().reset();
  }

  vector<PowerUp *> *availablePowerUps = EntityHandler::getInstance().getPowerUps();
  if (!availablePowerUps->empty() && ballIsOwned) {
    for (auto &powerUp: *availablePowerUps) {
      if (powerUp->isAssigned()) continue;

      if (powerUp->getGlobalBounds().intersects(body.getGlobalBounds())) {
        PowerUpEffect effectType = powerUp->getEffect();
        powerUp->setAssignedToPlayer();

        Player *player;
        if (effectType == PowerUpEffect::GOOD) {
          player = players->at(currentOwner);
        } else {
          currentOwner = currentOwner == players->size() - 1 ? 0 : currentOwner + 1;
          player = players->at(currentOwner);
        }

        SoundHandler::getInstance().playSound(GameSounds[COLLECT]);
        player->applyPowerUp(powerUp);
        break;
      }
    }
  }

  // Handle player collision
  std::vector<Player *>::iterator it;
  int index = 0;
  for (it = players->begin(); it != players->end(); ++it, ++index) {
    Player *player = *it;
    if (player->getGlobalBounds().intersects(body.getGlobalBounds())) {
      mt19937 engine = RandomEngine::getInstance().getEngine();
      uniform_int_distribution<> angleDist(0,10);

      angle = 180 - angle + static_cast<float>(angleDist(engine));
      SoundHandler::getInstance().playSound(GameSounds[BOUNCE_PLAYER]);
      EntityHandler::getInstance().setBallOwnerIndex(index);
      break;
    }
  }

  float angleRad = angle * 3.1415f / 180.0f;

  pos_x += speed * cos(angleRad);
  pos_y += speed * sin(angleRad);

  body.setPosition(pos_x, pos_y);
}

void Ball::draw() {
  window->draw(body);
}

void Ball::reset() {
  pos_y = static_cast<float>(static_cast<float>(window->getSize().y) / 2);
  pos_x = static_cast<float>(static_cast<float>(window->getSize().x) / 2);
  body.setPosition(pos_x, pos_y);
  mt19937 &engine = RandomEngine::getInstance().getEngine();
  uniform_int_distribution<> startingAngle(0, 360);

  angle = static_cast<float>(startingAngle(engine));
}

CircleShape *Ball::getBody() {
  return &body;
}

void Ball::setControlType(BallControl type) {
  this->control = type;
}

BallControl Ball::getControlType() {
  return control;
}
void Ball::setPosition(float x, float y) {
  pos_x = x;
  pos_y = y;
  body.setPosition(pos_x, pos_y);
}
