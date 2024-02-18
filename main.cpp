#include "Ball/Ball.h"
#include "Player/Player.h"
#include "PointCounter/PointCounter.h"
#include "PowerUp/GrowPowerUp.h"
#include "PowerUp/ShrinkPowerUp.h"
#include "PowerUp/SlowPowerUp.h"
#include "PowerUp/SpeedPowerUp.h"
#include "SoundHandler/SoundHandler.h"
#include "random"
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

PowerUp *createPowerUp(int type) {
  switch(type) {
    case 0: return new SpeedPowerUp();
    case 1: return new GrowPowerUp();
    case 2: return new ShrinkPowerUp();
    case 3: return new SlowPowerUp();
    default: return nullptr;
  }
}

PowerUp *getRandomPowerUp() {
  mt19937 &engine = RandomEngine::getInstance().getEngine();
  uniform_int_distribution<> distribution(0, 3);
  int type = distribution(engine);

  return createPowerUp(type);
}

int main() {

  RenderWindow *window = GameHandler::getInstance().getWindow();

  EntityHandler::getInstance().init_players();
  EntityHandler::getInstance().init_ball();
  EntityHandler::getInstance().init_pointCounter();

  SoundHandler::getInstance();
  TextureHandler::getInstance();

  vector<Player *> *players = EntityHandler::getInstance().getPlayers();
  Ball *ball = EntityHandler::getInstance().getBall();
  PointCounter *pointCounter = EntityHandler::getInstance().getPointCounter();
  vector<PowerUp *> *powerUps = EntityHandler::getInstance().getPowerUps();

  SoundHandler::getInstance().playMusic("../assets/sounds/background.wav");

  Clock clock;
  Clock powerUpClock;
  Time powerUpTime = seconds(5);

  auto menuHandler = *new MenuHandler();

  /**
  * Main Loop
  */
  while (window->isOpen()) {
    GameState gameState = GameHandler::getInstance().getGameState();
    Time deltaTime = clock.restart();

    Event event{};

    window->clear(Color::Black);

    switch (gameState) {
      case GameState::MENU:
        menuHandler.drawMenu(&event);
        break;
      case GameState::WAITING_FOR_CONNECTION:
        menuHandler.drawCreateOnline(&event);
        break;
      case GameState::CONNECT_TO_PLAYER:
        menuHandler.drawConnectOnline(&event);
        break;
      case GameState::PLAY:
        while (window->pollEvent(event)) {
          if (event.type == Event::Closed) {
            window->close();
          }
          if (event.type == Event::KeyPressed) {
            if (event.key.code == Keyboard::Enter) {
              players->at(1)->setControl(PlayerControl::AUTOMATIC);
            }
          }
        }

        for (int index = 0; index < players->size(); index++) {
          Player *player = players->at(index);
          player->draw();
          player->update(index, deltaTime);
        }

        ball->draw();
        ball->update();

        pointCounter->draw();

        for (auto powerUp: *powerUps) {
          powerUp->draw();
        }

        if (powerUpClock.getElapsedTime() > powerUpTime) {
          if (EntityHandler::getInstance().getCurrentBallOwnerIndex() != -1) {

            mt19937 &engine = RandomEngine::getInstance().getEngine();
            uniform_int_distribution<> pos_x_distribution(0, (int) window->getSize().x);
            uniform_int_distribution<> pos_y_distribution(0, (int) window->getSize().y);

            int x = pos_x_distribution(engine);
            int y = pos_y_distribution(engine);
            PowerUp *powerUp = getRandomPowerUp();
            if (powerUp == nullptr) return 0;
            powerUp->setPosition(Vector2f(x, y));
            EntityHandler::getInstance().addPowerUp(powerUp);
          }

          powerUpClock.restart();
        }
        break;
    }

    window->display();
  }

  return 0;
}