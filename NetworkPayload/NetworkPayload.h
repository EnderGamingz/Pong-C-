#ifndef PONG_NETWORKPAYLOAD_H
#define PONG_NETWORKPAYLOAD_H

#include "BasePayload.h"
#include "SFML/Graphics.hpp"
#include <SFML/Network.hpp>

using namespace sf;

class NetworkPayload : public BasePayload {
public:
  float ball_x{};
  float ball_y{};
  float host_y{};
  int player1_score{};
  int player2_score{};
  int player_owning_ball{};

  Packet pack() override;
  void unpack(Packet *packet) override;
};


#endif//PONG_NETWORKPAYLOAD_H
