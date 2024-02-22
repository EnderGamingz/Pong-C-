#ifndef PONG_NETWORKINGHANDLER_H
#define PONG_NETWORKINGHANDLER_H

#include "../NetworkPayload/NetworkPayload.h"
#include "SFML/Network.hpp"
#include "iostream"

using namespace std;
using namespace sf;

enum NetworkStatus {
  IDLE,
  LISTENING,
  AWAITING_CONNECTION,
  CONNECTING,
  CONNECTED,
  POST_CONNECTION,
  ERROR
};

enum NetworkRole {
  HOST,
  CLIENT
};

class NetworkingHandler {
public:
  NetworkingHandler();
  ~NetworkingHandler();

  bool listen(unsigned short port);
  NetworkStatus connect(const IpAddress &ip, unsigned short port);
  bool sendGameState(NetworkPayload &gameState);
  bool receiveGameState();
  NetworkStatus accept();
  void disconnect();

  NetworkStatus status = IDLE;
  NetworkRole role = CLIENT;

  NetworkPayload gameStateData{};

  int errorTimeout = 0;
  int errorTimeoutMax = 50;

private:
  TcpSocket socket;
  TcpListener listener;
};


#endif//PONG_NETWORKINGHANDLER_H
