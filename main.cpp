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
  switch (type) {
    case 0:
      return new SpeedPowerUp();
    case 1:
      return new GrowPowerUp();
    case 2:
      return new ShrinkPowerUp();
    case 3:
      return new SlowPowerUp();
    default:
      return nullptr;
  }
}

PowerUp *getRandomPowerUp() {
  mt19937 &engine = RandomEngine::getInstance().getEngine();
  uniform_int_distribution<> distribution(0, 3);
  int type = distribution(engine);

  return createPowerUp(type);
}

int main() {

  GameHandler &gameHandler = GameHandler::getInstance();
  EntityHandler &entityHandler = EntityHandler::getInstance();
  SoundHandler &soundHandler = SoundHandler::getInstance();
  TextureHandler::getInstance();
  MenuHandler &menuHandler = *new MenuHandler();

  RenderWindow *window = gameHandler.getWindow();

  entityHandler.init_players();
  entityHandler.init_ball();
  entityHandler.init_pointCounter();

  vector<Player *> *players = entityHandler.getPlayers();
  Ball *ball = entityHandler.getBall();
  PointCounter *pointCounter = entityHandler.getPointCounter();
  vector<PowerUp *> *powerUps = entityHandler.getPowerUps();

  soundHandler.playMusic("../assets/sounds/background.wav");

  Clock clock;
  Clock powerUpClock;
  Time powerUpTime = seconds(5);

  /**
  * Main Loop
  */
  while (window->isOpen()) {
    GameState gameState = gameHandler.getGameState();
    GameType gameType = gameHandler.gameType;
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
        if (gameType == GameType::LOCAL || gameType == GameType::ONLINE_HOST) {
          while (window->pollEvent(event)) {
            if (event.type == Event::Closed) {
              window->close();
            }
            if (event.type == Event::KeyPressed && gameType == GameType::LOCAL) {
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

          if (gameType == GameType::ONLINE_HOST) {
            auto ballPos = ball->getBody()->getPosition();
            NetworkPayload networkPayload{};
            networkPayload.ball_x = ballPos.x;
            networkPayload.ball_y = ballPos.y;

            vector<int> scores = entityHandler.getPointCounter()->getScores();
            networkPayload.player1_score = scores[0];
            networkPayload.player2_score = scores[1];

            gameHandler.networkingHandler->sendGameState(networkPayload);
          }

          pointCounter->draw();

          for (auto powerUp: *powerUps) {
            powerUp->draw();
          }

          if (powerUpClock.getElapsedTime() > powerUpTime && gameType == GameType::LOCAL) {
            if (entityHandler.getCurrentBallOwnerIndex() != -1) {

              mt19937 &engine = RandomEngine::getInstance().getEngine();
              uniform_int_distribution<> pos_x_distribution(0, (int) window->getSize().x);
              uniform_int_distribution<> pos_y_distribution(0, (int) window->getSize().y);

              int x = pos_x_distribution(engine);
              int y = pos_y_distribution(engine);
              PowerUp *powerUp = getRandomPowerUp();
              if (powerUp == nullptr) return 0;
              powerUp->setPosition(Vector2f((float) x, (float) y));
              entityHandler.addPowerUp(powerUp);
            }

            powerUpClock.restart();
          }
        } else if (gameType == GameType::ONLINE) {
          gameHandler.networkingHandler->receiveGameState();
          NetworkPayload *gameStateData = &gameHandler.networkingHandler->gameStateData;
          while (window->pollEvent(event)) {
            if (event.type == Event::Closed) {
              window->close();
            }
          }

          ball->draw();
          ball->setPosition(gameStateData->ball_x, gameStateData->ball_y);
          entityHandler.getPointCounter()->setScoresFromParams(gameStateData->player1_score, gameStateData->player2_score);

          pointCounter->draw();

        }
        break;
    }
    window->display();
  }

  return 0;
}