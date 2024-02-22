#include "Player.h"

Player::Player(PlayerSide side, Keyboard::Key up, Keyboard::Key down) {
  this->window = GameHandler::getInstance().getWindow();
  this->pos_y = static_cast<float>(this->window->getSize().y / 2);// NOLINT(*-integer-division)
  RectangleShape _body;
  _body.setSize(Vector2f(width, height));
  _body.setOrigin(0.0f, height / 2);
  _body.setFillColor(Color::White);

  float _pos_x;
  switch (side) {
    case PlayerSide::LEFT:
      _pos_x = playerOffset;
      break;
    case PlayerSide::RIGHT:
      _pos_x = static_cast<float>(this->window->getSize().x) - playerOffset - width;
      break;
  }

  _body.setPosition(Vector2f(_pos_x, pos_y));
  this->body = _body;

  this->up_key = up;
  this->down_Key = down;

  this->side = side;
}

void Player::update(int index, Time deltaTime) {
  if (isOwningBall(index)) body.setFillColor(Color::Red);
  else
    body.setFillColor(Color::White);

  for (PowerUp *powerUp: activePowerUps) {
    powerUp->decrementTime(deltaTime);
  }

  removeExpiredPowerUps();

  for (PowerUp *powerUp: activePowerUps) {
    powerUp->applyTo(this);
  }

  body.setSize(Vector2f(width, height));
  body.setOrigin(0.0f, height / 2);

  Vector2<unsigned int> windowSize = this->window->getSize();
  Vector2f playerPosition = body.getPosition();
  if (control == PlayerControl::MANUAL) {
    if (Keyboard::isKeyPressed(up_key) && playerPosition.y >= 0 + body.getSize().y / 2) {
      body.move(0, -speed);
    }
    if (Keyboard::isKeyPressed(down_Key) && playerPosition.y < static_cast<float>(windowSize.y) - body.getSize().y / 2) {
      body.move(0, speed);
    }
  } else if (control == PlayerControl::AUTOMATIC) {
    auto ball = EntityHandler::getInstance().getBallPosition();
    // Calculate difference in y positions
    float yDiff = ball.y - playerPosition.y;

    if (yDiff < 0 && playerPosition.y >= 0 + body.getSize().y / 2) {
      body.move(0, -speed);
    } else if (yDiff > 0 && playerPosition.y < static_cast<float>(windowSize.y) - body.getSize().y / 2) {
      body.move(0, speed);
    }
  }
}


bool Player::isOwningBall(int index) {
  return EntityHandler::getInstance().getCurrentBallOwnerIndex() == index;
}

void Player::draw() {
  window->draw(body);
}

PlayerSide Player::getSide() {
  return side;
}

FloatRect Player::getGlobalBounds() {
  return body.getGlobalBounds();
}

void Player::reset() {
  float _pos_x;
  switch (side) {
    case PlayerSide::LEFT:
      _pos_x = playerOffset;
      break;
    case PlayerSide::RIGHT:
      _pos_x = static_cast<float>(this->window->getSize().x) - playerOffset - width;
      break;
  }

  body.setPosition(Vector2f(_pos_x, pos_y));
}

void Player::applyPowerUp(PowerUp *powerUp) {
  activePowerUps.push_back(powerUp);
}

RectangleShape *Player::getBody() {
  return &body;
}

void Player::setSpeed(float _speed) {
  this->speed = _speed;
}

/**
 * @brief Removes expired power-ups from the active power-up list
 *
 * This function iterates through the active power-ups vector and removes any power-ups
 * that have expired. A power-up is considered expired if its time remaining is zero.
 * After removing the expired power-ups, the vector is updated to remove the empty slots.
 *
 * @return void
 */
void Player::removeExpiredPowerUps() {
  const vector<PowerUp *>::iterator &expiredPowerUpsIterator =
      std::remove_if(activePowerUps.begin(),
                     activePowerUps.end(),
                     [this](const auto &powerUp) {
                       if (powerUp->isExpired()) {
                         powerUp->revert(this);
                         return true;
                       } else {
                         return false;
                       }
                     });

  activePowerUps.erase(
      expiredPowerUpsIterator,
      activePowerUps.end());
}

void Player::setHeight(float _height) {
  this->height = _height;
}

void Player::setControl(PlayerControl _control) {
  this->control = _control;
}

PlayerControl Player::getControl() {
  return control;
}
