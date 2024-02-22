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

void handleOnline(GameHandler &gameHandler, EntityHandler &entityHandler, RenderWindow *window, vector<Player *> *players, Ball *ball, PointCounter *pointCounter, GameType &gameType, Time &deltaTime, Event &event);
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

Player *getFirstPlayerWithControlType(vector<Player *> *players, PlayerControl control) {
  for (auto player: *players) {
    if (player->getControl() == control) {
      return player;
    }
  }
  return nullptr;
}

void handleKeyPressEvent(vector<Player *> *players, Event &event) {
  if (event.key.code == Keyboard::Enter)
    players->at(1)->setControl(PlayerControl::AUTOMATIC);
}

void handlePollEvent(RenderWindow *window, Event &event, GameType gameType, vector<Player *> *players) {
  while (window->pollEvent(event)) {
    if (event.type == Event::Closed) window->close();
    if (Keyboard::isKeyPressed(Keyboard::Escape)) GameHandler::getInstance().setGameState(GameState::MENU);
    else if (event.type == Event::KeyPressed && gameType == GameType::LOCAL)
      handleKeyPressEvent(players, event);
  }
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
        menuHandler.drawConnectOnlineInput(&event);
        break;
      case GameState::CONNECTING_TO_PLAYER:
        menuHandler.drawConnectOnline(&event);
        break;
      case GameState::PLAY:
        if (gameType == GameType::LOCAL || gameType == GameType::ONLINE_HOST) {

          handlePollEvent(window, event, gameType, players);

          for (int index = 0; index < players->size(); index++) {
            Player *player = players->at(index);
            player->draw();
            player->update(index, deltaTime);
          }

          if (gameType == ONLINE_HOST) {
            gameHandler.networkingHandler->receiveClientState();
            ClientToHostPayload clientPayload = gameHandler.networkingHandler->clientStateData;
            Player *remotePlayer = getFirstPlayerWithControlType(players, PlayerControl::REMOTE);
            if (remotePlayer != nullptr) {
              remotePlayer->getBody()->setPosition(remotePlayer->getBody()->getPosition().x, clientPayload.client_y);
            }
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

            Player *playerWithControl = getFirstPlayerWithControlType(players, PlayerControl::MANUAL);
            if (playerWithControl != nullptr) {
              auto playerPos = playerWithControl->getBody()->getPosition();
              networkPayload.host_y = playerPos.y;
            }

            networkPayload.player_owning_ball = entityHandler.getCurrentBallOwnerIndex();

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
          handleOnline(gameHandler, entityHandler, window, players, ball, pointCounter, gameType, deltaTime, event);
        }
        break;
    }
    window->display();
  }

  return 0;
}
void handleOnline(GameHandler &gameHandler, EntityHandler &entityHandler, RenderWindow *window, vector<Player *> *players, Ball *ball, PointCounter *pointCounter, GameType &gameType, Time &deltaTime, Event &event) {
  gameHandler.networkingHandler->receiveGameState();
  NetworkPayload *gameStateData = &gameHandler.networkingHandler->gameStateData;

  handlePollEvent(window, event, gameType, players);

  ball->draw();
  ball->setPosition(gameStateData->ball_x, gameStateData->ball_y);
  entityHandler.getPointCounter()->setScoresFromParams(gameStateData->player1_score, gameStateData->player2_score);
  entityHandler.setBallOwnerIndex(gameStateData->player_owning_ball);

  pointCounter->draw();

  for (int index = 0; index < players->size(); index++) {
    Player *player = players->at(index);

    // Update the remote player position
    if (player->getControl() == REMOTE) {
      RectangleShape *playerBody = player->getBody();
      playerBody->setPosition(playerBody->getPosition().x, gameStateData->host_y);
    }

    player->update(index, deltaTime);
    player->draw();
  }

  // Send client player data to Host
  ClientToHostPayload clientPayload = gameHandler.networkingHandler->clientStateData;
  Player *manualPlayer = getFirstPlayerWithControlType(players, MANUAL);
  if (manualPlayer != nullptr) {
    clientPayload.client_y = manualPlayer->getBody()->getPosition().y;
  }

  gameHandler.networkingHandler->sendClientState(clientPayload);
}
