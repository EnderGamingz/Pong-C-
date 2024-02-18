#include "MenuHandler.h"
#include "../GameHandler/GameHandler.h"

MenuHandler::MenuHandler() {
  this->window = GameHandler::getInstance().getWindow();
  this->font = GameHandler::getInstance().getFont();

  Text _title = Text("Ultimate Super Mega Pong", font, 30);
  _title.setFillColor(sf::Color::White);
  _title.setPosition({window->getSize().x / 2 - _title.getLocalBounds().width / 2, 10});
  this->title = _title;

  Text _playLocal = Text("Play Local", font, 30);
  _playLocal.setFillColor(sf::Color::White);
  _playLocal.setPosition({window->getSize().x / 2 - _playLocal.getLocalBounds().width / 2, 100});
  _playLocal.setStyle(sf::Text::Bold);
  this->playLocal = _playLocal;

  Text _playOnline = Text("Connect Online", font, 30);
  _playOnline.setFillColor(sf::Color::White);
  _playOnline.setPosition({window->getSize().x / 2 - _playOnline.getLocalBounds().width / 2, 150});
  this->playOnline = _playOnline;

  Text _createOnline = Text("Create Online", font, 30);
  _createOnline.setFillColor(sf::Color::White);
  _createOnline.setPosition({window->getSize().x / 2 - _createOnline.getLocalBounds().width / 2, 200});
  this->createOnline = _createOnline;

  Text _quit = Text("Quit", font, 30);
  _quit.setFillColor(sf::Color::White);
  _quit.setPosition({window->getSize().x / 2 - _quit.getLocalBounds().width / 2, 250});
  this->quit = _quit;

  Text _testButton = Text("Send test message", font, 30);
  _testButton.setFillColor(sf::Color::White);
  _testButton.setPosition({window->getSize().x / 2 - _testButton.getLocalBounds().width / 2, 300});
  this->testButton = _testButton;
}

void MenuHandler::drawMenu(Event *event) {
  this->window->draw(this->title);
  this->window->draw(this->playLocal);
  this->window->draw(this->playOnline);
  this->window->draw(this->createOnline);
  this->window->draw(this->quit);
  while (window->pollEvent(*event)) {
    if (event->type == Event::Closed) {
      window->close();
    }
    if (event->type == Event::MouseButtonPressed) {
      if (this->playLocal.getGlobalBounds().contains(event->mouseButton.x, event->mouseButton.y)) {
        GameHandler::getInstance().setGameState(GameState::PLAY);
        GameHandler::getInstance().setGameType(GameType::LOCAL);

      } else if (this->playOnline.getGlobalBounds().contains(event->mouseButton.x, event->mouseButton.y)) {
        GameHandler::getInstance().setGameState(GameState::CONNECT_TO_PLAYER);
        GameHandler::getInstance().setGameType(GameType::ONLINE);

      } else if (this->createOnline.getGlobalBounds().contains(event->mouseButton.x, event->mouseButton.y)) {
        GameHandler::getInstance().setGameState(GameState::WAITING_FOR_CONNECTION);
        GameHandler::getInstance().setGameType(GameType::ONLINE);
        GameHandler::getInstance().onlineListen();


      } else if (this->quit.getGlobalBounds().contains(event->mouseButton.x, event->mouseButton.y)) {
        window->close();
      }
    }
  }
}
void MenuHandler::drawCreateOnline(Event *event) {
  GameHandler::getInstance().onlineAccept();
  NetworkingHandler *networkingHandlerInstance = GameHandler::getInstance().networkingHandler;
  NetworkStatus connectionStatus = networkingHandlerInstance->status;
  NetworkRole role = networkingHandlerInstance->role;
  switch (connectionStatus) {
    case NetworkStatus::CONNECTED:
      title.setString("Other player connected");
      break;
    case NetworkStatus::LISTENING:
    case NetworkStatus::AWAITING_CONNECTION:
      title.setString("Waiting for other player");
      break;
    case NetworkStatus::ERROR:
      title.setString("Error Connecting");
      break;
  }

  title.setPosition({window->getSize().x / 2 - title.getLocalBounds().width / 2, 10});
  this->window->draw(this->title);
  if (connectionStatus == NetworkStatus::CONNECTED && role == NetworkRole::HOST) {
    this->window->draw(this->testButton);
  }

  while (window->pollEvent(*event)) {
    if (event->type == Event::Closed) {
      window->close();
    }
    if (event->type == Event::MouseButtonPressed) {
      if (this->testButton.getGlobalBounds().contains(event->mouseButton.x, event->mouseButton.y)) {
        NetworkPayload payload = {20.0f, 20.0f};
        if (!networkingHandlerInstance->sendGameState(payload)) {
          networkingHandlerInstance->status = NetworkStatus::ERROR;
        }
      }
    }
  }
}

void MenuHandler::drawConnectOnline(Event *event) {
  GameHandler::getInstance().onlineConnect();
  NetworkStatus connectionStatus = GameHandler::getInstance().networkingHandler->status;
  switch (connectionStatus) {
    case NetworkStatus::CONNECTED:
      title.setString("Connected to other player");
      break;
    case NetworkStatus::CONNECTING:
      title.setString("Connecting to other player");
      break;
    case NetworkStatus::ERROR:
      title.setString("Error Connecting");
      break;
  }
  title.setPosition({window->getSize().x / 2 - title.getLocalBounds().width / 2, 10});
  this->window->draw(this->title);
  while (window->pollEvent(*event)) {
    if (event->type == Event::Closed) {
      window->close();
    }
  }
}
