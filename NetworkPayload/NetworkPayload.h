#ifndef PONG_NETWORKPAYLOAD_H
#define PONG_NETWORKPAYLOAD_H

#include "SFML/Graphics.hpp"
#include <SFML/Network.hpp>

using namespace sf;

struct NetworkPayload
{
  float ball_x;
  float ball_y;
};

sf::Packet& operator<<(sf::Packet& packet, const NetworkPayload& data);
sf::Packet& operator>>(sf::Packet& packet, NetworkPayload& data);


#endif//PONG_NETWORKPAYLOAD_H
