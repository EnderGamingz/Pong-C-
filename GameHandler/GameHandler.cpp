#include "GameHandler.h"
#include "../PointCounter/PointCounter.h"
#include <iostream>

GameHandler::GameHandler() {
  cout << "GameHandler: created" << endl;
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

  cout << "GameHandler: Font loaded" << endl;

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

GameState GameHandler::getGameState() {
  return this->gameState;
}

void GameHandler::setGameType(GameType type) {
  this->gameType = type;
}

GameType GameHandler::getGameType() {
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
  if (networkingHandler->status != NetworkStatus::CONNECTED) {
    networkingHandler->status = networkingHandler->accept();
  }
  return networkingHandler->status;
}

void GameHandler::onlineConnect() {
  NetworkStatus status = networkingHandler->status;
  if (status == NetworkStatus::CONNECTED) {
    networkingHandler->receiveGameState();
    this->gameStateData = networkingHandler->gameStateData;
  }
  if (status == NetworkStatus::IDLE || status == NetworkStatus::CONNECTING) {
    networkingHandler->role = NetworkRole::CLIENT;
    networkingHandler->status = networkingHandler->connect("127.0.0.1", 6995);
  }
}

void GameHandler::onlineDisconnect() {
  if (networkingHandler->status == NetworkStatus::CONNECTED) {
    networkingHandler->status = NetworkStatus::IDLE;
    networkingHandler->disconnect();
  }
}