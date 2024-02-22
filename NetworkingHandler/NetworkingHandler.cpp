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
  return NetworkStatus::POST_CONNECTION;
}

bool NetworkingHandler::sendGameState(BasePayload &gameState) {
  if (status != NetworkStatus::CONNECTED) return false;
  Packet packet = gameState.pack();

  if (socket.send(packet) != sf::Socket::Done) {
    cout << "Networking Handler: Failed to send packet" << endl;
    if (errorTimeout < errorTimeoutMax) {
      errorTimeout++;
    } else {
      errorTimeout = 0;
      cout << "Networking Handler: Timed out sending packet" << endl;
      status = NetworkStatus::ERROR;
    }
    return false;
  }
  return true;
}

bool NetworkingHandler::sendClientState(BasePayload &clientState) {
  if (status != NetworkStatus::CONNECTED) return false;
  Packet packet = clientState.pack();

  if (socket.send(packet) != sf::Socket::Done) {
    cout << "Networking Handler: Failed to send packet" << endl;
    if (errorTimeout < errorTimeoutMax) {
      errorTimeout++;
    } else {
      errorTimeout = 0;
      cout << "Networking Handler: Timed out sending packet" << endl;
      status = NetworkStatus::ERROR;
    }
    return false;
  }
  return true;
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
  return NetworkStatus::POST_CONNECTION;
}

bool NetworkingHandler::receiveGameState() {
  Packet packet;
  sf::Socket::Status socketStatus = socket.receive(packet);
  if (socketStatus == sf::Socket::NotReady) return true;
  if (socketStatus == sf::Socket::Error || socketStatus == sf::Socket::Disconnected) {
    cout << "Networking Handler: Failed to receive packet" << endl;
    if (errorTimeout < errorTimeoutMax) {
      errorTimeout++;
    } else {
      errorTimeout = 0;
      cout << "Networking Handler: Timed out receiving packet" << endl;
      status = NetworkStatus::ERROR;
    }
    return false;
  }

  NetworkPayload receivedGameState;
  receivedGameState.unpack(&packet);

  cout << "Networking Handler: Received packet with game state" << endl;

  gameStateData = receivedGameState;

  return true;
}

bool NetworkingHandler::receiveClientState() {
  Packet packet;
  sf::Socket::Status socketStatus = socket.receive(packet);
  if (socketStatus == sf::Socket::NotReady) return true;
  if (socketStatus == sf::Socket::Error || socketStatus == sf::Socket::Disconnected) {
    cout << "Networking Handler: Failed to receive packet" << endl;
    if (errorTimeout < errorTimeoutMax) {
      errorTimeout++;
    } else {
      errorTimeout = 0;
      cout << "Networking Handler: Timed out receiving packet" << endl;
      status = NetworkStatus::ERROR;
    }
    return false;
  }

  ClientToHostPayload receivedClientState;
  receivedClientState.unpack(&packet);

  clientStateData = receivedClientState;

  cout << "Networking Handler: Received packet with client state" << endl;

  return true;
}
