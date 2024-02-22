#ifndef PONG_CLIENTTOHOST_H
#define PONG_CLIENTTOHOST_H

#include "BasePayload.h"
#include "SFML/Graphics.hpp"
#include <SFML/Network.hpp>

using namespace sf;

class ClientToHostPayload : public BasePayload {
public:
  float client_y{};

  Packet pack() override;
  void unpack(Packet *packet) override;
};


#endif//PONG_CLIENTTOHOST_H
