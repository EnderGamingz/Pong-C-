#include "NetworkingHandler.h"

NetworkingHandler::NetworkingHandler() {
  socket.setBlocking(true);
  listener.setBlocking(false);
}

NetworkingHandler::~NetworkingHandler() {
  socket.disconnect();
  listener.close();
}

void NetworkingHandler::disconnect() {
  socket.disconnect();
}


bool NetworkingHandler::listen(unsigned short port) {
  cout << "Networking Handler: Trying to listen on port " << port << endl;
  if (listener.listen(port) != sf::Socket::Done) {
    cout << "Networking Handler: Failed to listen on port " << port << endl;
    return false;
  }
  cout << "Networking Handler: Listening on port " << port << endl;
  return true;
}

NetworkStatus NetworkingHandler::accept() {
  Socket::Status socketStatus = listener.accept(socket);
  if (socketStatus == sf::Socket::Error) {
    cout << "Networking Handler: Failed to accept connection" << endl;
    return NetworkStatus::ERROR;
  }
  if (socketStatus == Socket::NotReady) return NetworkStatus::AWAITING_CONNECTION;
  cout << "Networking Handler: Connection accepted" << endl;
  return NetworkStatus::CONNECTED;
}

void NetworkingHandler::sendGameState(const sf::String &gameState) {
  Packet packet;
  packet << gameState;
  if (socket.send(packet) != sf::Socket::Done) {
    cout << "Networking Handler: Failed to send packet" << endl;
    return;
  }
}

NetworkStatus NetworkingHandler::connect(const IpAddress &ip, unsigned short port) {
  cout << "Networking Handler: Trying to connect to " << ip << " on port " << port << endl;
  Socket::Status connectionStatus = socket.connect(ip, port);
  if (connectionStatus == sf::Socket::Error) {
    cout << "Networking Handler: Failed to connect" << endl;
    return NetworkStatus::ERROR;
  }
  if (connectionStatus == Socket::NotReady) return NetworkStatus::CONNECTING;
  cout << "Networking Handler: Connected" << endl;
  socket.setBlocking(false);
  return NetworkStatus::CONNECTED;
}

void NetworkingHandler::receiveGameState() {
  Packet packet;
  sf::Socket::Status socketStatus = socket.receive(packet);
  if (socketStatus == sf::Socket::NotReady) return;
  if (socketStatus == sf::Socket::Error || socketStatus == sf::Socket::Disconnected) {
    cout << "Networking Handler: Failed to receive packet" << endl;
    return;
  }

  sf::String receivedGameState;
  if (packet >> receivedGameState) {
    cout << "Networking Handler: Received packet with game state: " << receivedGameState.toAnsiString() << endl;
    gameStateData = receivedGameState;
  } else {
    cout << "Networking Handler: Failed to extract game state from packet" << endl;
  }
}