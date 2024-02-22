#include "MenuHandler.h"
#include "../GameHandler/GameHandler.h"

MenuHandler::MenuHandler() {
  this->window = GameHandler::getInstance().getWindow();
  this->font = GameHandler::getInstance().getFont();

  this->title = Text("Ultimate Super Mega Pong", font, 30);
  this->title.setFillColor(sf::Color::White);
  const Vector2u &windowSize = window->getSize();

  this->windowCenterX = (float) windowSize.x / 2;
  this->title.setPosition({windowCenterX - title.getLocalBounds().width / 2, 10});

  this->playLocal = Text("Play Local", font, 30);
  this->playLocal.setFillColor(sf::Color::White);
  this->playLocal.setPosition({windowCenterX - playLocal.getLocalBounds().width / 2, 100});
  this->playLocal.setStyle(sf::Text::Bold);

  this->playOnline = Text("Connect Online", font, 30);
  this->playOnline.setFillColor(sf::Color::White);
  this->playOnline.setPosition({windowCenterX - playOnline.getLocalBounds().width / 2, 150});

  this->createOnline = Text("Create Online", font, 30);
  this->createOnline.setFillColor(sf::Color::White);
  this->createOnline.setPosition({windowCenterX - createOnline.getLocalBounds().width / 2, 200});

  this->quit = Text("Quit", font, 30);
  this->quit.setFillColor(sf::Color::White);
  this->quit.setPosition({windowCenterX - quit.getLocalBounds().width / 2, 250});

  this->userIpText = Text(GameHandler::getInstance().networkingHandler->getIpAddress(), font, 30);
  this->userIpText.setFillColor(sf::Color::White);
  this->userIpText.setPosition({windowCenterX - this->userIpText.getLocalBounds().width / 2, 250});

  this->buttonText = Text("Connect", font, 30);
  this->buttonText.setFillColor(sf::Color::Black);
  this->buttonText.setPosition({windowCenterX - buttonText.getLocalBounds().width / 2, 350});

  this->button = RectangleShape({buttonText.getLocalBounds().width + 40, buttonText.getLocalBounds().height + 20});
  this->button.setFillColor(sf::Color::White);
  this->button.setOutlineThickness(1);
  this->button.setOutlineColor(sf::Color::Black);
  this->button.setPosition({windowCenterX - button.getSize().x / 2, 350});

  this->inputBox = RectangleShape({userIpText.getLocalBounds().width + 40, userIpText.getLocalBounds().height + 20});
  this->inputBox.setFillColor(sf::Color(100, 100, 100));
  this->inputBox.setOutlineThickness(1);
  this->inputBox.setOutlineColor(sf::Color::White);
  this->inputBox.setPosition({windowCenterX - inputBox.getSize().x / 2, 250});
}

void MenuHandler::drawMenu(Event *event) {
  GameHandler &gameHandler = GameHandler::getInstance();

  title.setString("Ultimate Super Mega Pong");
  title.setPosition({windowCenterX - title.getLocalBounds().width / 2, 10});

  this->window->draw(this->title);
  this->window->draw(this->playLocal);
  this->window->draw(this->playOnline);
  this->window->draw(this->createOnline);
  this->window->draw(this->quit);

  while (window->pollEvent(*event)) {
    handleDefaultEvent(event, &gameHandler);
    if (event->type == Event::MouseButtonPressed) {
      auto mouse_x = (float) event->mouseButton.x;
      auto mouse_y = (float) event->mouseButton.y;

      if (this->playLocal.getGlobalBounds().contains(mouse_x, mouse_y)) {
        gameHandler.setGameState(GameState::PLAY);
        gameHandler.setGameType(GameType::LOCAL);

      } else if (this->playOnline.getGlobalBounds().contains(mouse_x, mouse_y)) {
        gameHandler.setGameState(GameState::CONNECT_TO_PLAYER);
        gameHandler.setGameType(GameType::ONLINE);

      } else if (this->createOnline.getGlobalBounds().contains(mouse_x, mouse_y)) {
        gameHandler.setGameState(GameState::WAITING_FOR_CONNECTION);
        gameHandler.setGameType(GameType::ONLINE);
        gameHandler.onlineListen();

      } else if (this->quit.getGlobalBounds().contains(mouse_x, mouse_y)) {
        window->close();
      }
    }
  }
}
void MenuHandler::drawCreateOnline(Event *event) {
  GameHandler &gameHandler = GameHandler::getInstance();
  gameHandler.onlineAccept();

  NetworkingHandler *networkingHandlerInstance = gameHandler.networkingHandler;
  NetworkStatus connectionStatus = networkingHandlerInstance->status;
  switch (connectionStatus) {
    case NetworkStatus::CONNECTED:
      title.setString("TCP connection established");
      break;
    case NetworkStatus::CONNECTING:
      title.setString("TCP Connection in progress");
      break;
    case NetworkStatus::LISTENING:
    case NetworkStatus::AWAITING_CONNECTION:
      title.setString("Waiting for TCP connection from " + NetworkingHandler::getLocalIpAddress());
      break;
    case NetworkStatus::ERROR:
      title.setString("Socket Error while accepting connection");
      break;
    default:
      // No connection
      break;
  }

  title.setPosition({windowCenterX - title.getLocalBounds().width / 2, 10});
  this->window->draw(this->title);

  while (window->pollEvent(*event)) {
    handleDefaultEvent(event, &gameHandler);
  }
}

void MenuHandler::drawConnectOnline(Event *event) {
  GameHandler &gameHandler = GameHandler::getInstance();
  gameHandler.onlineConnect();
  NetworkStatus connectionStatus = gameHandler.networkingHandler->status;
  const String &networkIpAddress = gameHandler.networkingHandler->getIpAddress();

  switch (connectionStatus) {
    case NetworkStatus::CONNECTED:
      title.setString("TCP connection established with " + networkIpAddress);
      break;
    case NetworkStatus::CONNECTING:
      title.setString("TCP Connection to " + networkIpAddress + " in progress...");
      break;
    case NetworkStatus::ERROR:
      title.setString("Socket Error while connecting to " + networkIpAddress + ", try again");
      break;
    default:
      title.setString("Waiting for TCP connection to " + networkIpAddress + "...");
      break;
  }

  title.setPosition({windowCenterX - title.getLocalBounds().width / 2, 10});
  this->window->draw(this->title);

  while (window->pollEvent(*event)) {
    handleDefaultEvent(event, &gameHandler);
  }
}

void MenuHandler::handleDefaultEvent(Event *event, GameHandler *gameHandler) {
  if (event->type == Event::Closed) {
    window->close();
  }
  if (Keyboard::isKeyPressed(Keyboard::Escape)) {
    gameHandler->setGameState(MENU);
  }
}

void MenuHandler::updateIpText(Event *event) {
  if (event->type == Event::TextEntered) {
    //check for the escape key
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
      GameHandler::getInstance().setGameState(MENU);
      return;
    }

    Event::TextEvent &inputEvent = event->text;
    String &ipAddress = this->userIp;
    if (inputEvent.unicode < 128) {

      // Handle backspace
      if (inputEvent.unicode == 8) {
        if (ipAddress.getSize() > 0) {
          ipAddress.erase(ipAddress.getSize() - 1, 1);
        }
      } else {
        // Add character to the end
        ipAddress += static_cast<char>(inputEvent.unicode);
      }

      this->userIpText.setString(ipAddress);
      this->userIpText.setPosition({windowCenterX - this->userIpText.getLocalBounds().width / 2, 250});
    }
  }
}

void MenuHandler::drawConnectOnlineInput(Event *event) {
  title.setString("Enter IP address of Local Network");
  title.setPosition({windowCenterX - title.getLocalBounds().width / 2, 10});

  this->window->draw(this->button);
  this->window->draw(this->buttonText);

  this->window->draw(title);
  this->inputBox.setSize({userIpText.getLocalBounds().width, 40});
  this->inputBox.setPosition({windowCenterX - userIpText.getLocalBounds().width / 2, 250});
  this->window->draw(this->inputBox);
  this->window->draw(this->userIpText);

  while (window->pollEvent(*event)) {
    handleDefaultEvent(event, &GameHandler::getInstance());

    this->updateIpText(event);

    if (event->type == Event::KeyPressed) {
      if (event->key.code == Keyboard::Enter) {
        GameHandler &gameHandler = GameHandler::getInstance();
        if (this->userIp == "") {
          gameHandler.networkingHandler->setIpAddress("127.0.0.1");
        } else {
          gameHandler.networkingHandler->setIpAddress(this->userIp);
        }
        gameHandler.setGameState(GameState::CONNECTING_TO_PLAYER);
        gameHandler.setGameType(GameType::ONLINE);
      }
    }
    if (event->type == Event::MouseButtonPressed) {
      auto mouse_x = (float) event->mouseButton.x;
      auto mouse_y = (float) event->mouseButton.y;
      if (this->button.getGlobalBounds().contains(mouse_x, mouse_y)) {
        GameHandler &gameHandler = GameHandler::getInstance();
        gameHandler.setGameState(GameState::CONNECTING_TO_PLAYER);
        gameHandler.setGameType(GameType::ONLINE);
      }
    }
  }
}
