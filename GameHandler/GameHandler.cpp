#include "GameHandler.h"
#include "../Ball/Ball.h"
#include "../PointCounter/PointCounter.h"
#include <iostream>

GameHandler::GameHandler() {
  cout << "GameHandler: Created" << endl;
  this->window_height = 600;
  this->window_width = 800;
  // Initialize window
  ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  this->window.create(VideoMode(this->window_width, this->window_height), "Pong", Style::Default, settings);
  this->window.setFramerateLimit(60);

  cout << "GameHandler: Window created" << endl;


  // Initialize font
  if (!this->main_font.loadFromFile("../assets/roboto.ttf"))
    exit(1);

  cout << "GameHandler: Main Font loaded" << endl;

  NetworkingHandler &_networkingHandler = *new NetworkingHandler();
  this->networkingHandler = &_networkingHandler;
  cout << "GameHandler: NetworkHandler loaded" << endl;
}

GameHandler &GameHandler::getInstance() {
  static GameHandler instance;
  return instance;
}

RenderWindow *GameHandler::getWindow() {
  return &this->window;
}

Font &GameHandler::getFont() {
  return this->main_font;
}

void GameHandler::setGameState(GameState state) {
  this->gameState = state;
}

GameState GameHandler::getGameState() const {
  return this->gameState;
}

void GameHandler::setGameType(GameType type) {
  this->gameType = type;
}

GameType GameHandler::getGameType() const {
  return this->gameType;
}

void GameHandler::onlineListen() const {
  if (networkingHandler->status == NetworkStatus::IDLE) {
    networkingHandler->role = NetworkRole::HOST;
    if (networkingHandler->listen(6995)) {
      networkingHandler->status = NetworkStatus::LISTENING;
    } else {
      networkingHandler->status = NetworkStatus::ERROR;
    }
  }
}

NetworkStatus GameHandler::onlineAccept() const {
  NetworkStatus status = networkingHandler->status;
  if (status == NetworkStatus::POST_CONNECTION) {
    // Prepare for online game start for the Host
    printf("GameHandler: Preparing for online game start\n");
    EntityHandler &entityHandler = EntityHandler::getInstance();
    GameHandler &gameHandler = GameHandler::getInstance();
    entityHandler.getPlayers()->at(1)->setControl(PlayerControl::REMOTE);

    gameHandler.setGameState(GameState::PLAY);
    gameHandler.setGameType(GameType::ONLINE_HOST);

    networkingHandler->status = NetworkStatus::CONNECTED;
  } else if (status != NetworkStatus::CONNECTED) {
    networkingHandler->status = networkingHandler->accept();
    if (status == NetworkStatus::CONNECTED) {
      GameHandler &gameHandler = GameHandler::getInstance();
      gameHandler.setGameType(GameType::ONLINE_HOST);
      gameHandler.setGameState(GameState::PLAY);
    }
  }
  return networkingHandler->status;
}


/**
 * @brief Connects a Client to the Host.
 *
 * @details
 * - If the network status is POST_CONNECTION, it prepares for the online game start by setting the network status to CONNECTED.
 * - If the network status is CONNECTED, it receives the game state from the server and updates the local game state data.
 * - If the network status is IDLE or CONNECTING, it sets the network role to CLIENT and connects to the server with the given IP address and port.
 *
 * @note This function assumes that the networkingHandler object has been initialized and points to a valid instance.
 *
 * @note This function modifies the networkingHandler object and updates the network status and game state data.
 */
void GameHandler::onlineConnect() {
  NetworkStatus status = networkingHandler->status;
  if (status == NetworkStatus::POST_CONNECTION) {
    // Prepare for online game start for the Client
    printf("GameHandler: Preparing for online game start\n");
    EntityHandler &entityHandler = EntityHandler::getInstance();
    GameHandler &gameHandler = GameHandler::getInstance();
    entityHandler.getBall()->setControlType(BallControl::REMOTE_CONTROL);
    entityHandler.getPlayers()->at(0)->setControl(PlayerControl::REMOTE);

    gameHandler.setGameState(GameState::PLAY);
    gameHandler.setGameType(GameType::ONLINE);

    networkingHandler->status = NetworkStatus::CONNECTED;
  }
  if (status == NetworkStatus::CONNECTED) {
    if (!networkingHandler->receiveGameState()) {
      networkingHandler->status = NetworkStatus::ERROR;
    }
    this->gameStateData = &networkingHandler->gameStateData;
  }
  if (status == NetworkStatus::IDLE || status == NetworkStatus::CONNECTING) {
    networkingHandler->role = NetworkRole::CLIENT;
    networkingHandler->status = networkingHandler->connect("127.0.0.1", 6995);
  }
}

void GameHandler::onlineDisconnect() const {
  if (networkingHandler->status == NetworkStatus::CONNECTED) {
    networkingHandler->status = NetworkStatus::IDLE;
    networkingHandler->disconnect();
  }
}