#ifndef PONG_BASEPAYLOAD_H
#define PONG_BASEPAYLOAD_H

#include "SFML/Network.hpp"

using namespace sf;

class BasePayload {
public:
  virtual Packet pack() = 0;
  virtual void unpack(Packet *packet) = 0;
};

#endif//PONG_BASEPAYLOAD_H
