#ifndef PONG_NETWORKINGHANDLER_H
#define PONG_NETWORKINGHANDLER_H

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
  void sendGameState(const String &gameState);
  void receiveGameState();
  NetworkStatus accept();
  void disconnect();

  NetworkStatus status = IDLE;
  NetworkRole role = CLIENT;

  String gameStateData = "";

private:
  TcpSocket socket;
  TcpListener listener;

};


#endif//PONG_NETWORKINGHANDLER_H
