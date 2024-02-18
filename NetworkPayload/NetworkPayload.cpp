#include "NetworkPayload.h"

Packet& operator<<(sf::Packet& packet, const NetworkPayload& data) {
  return packet << data.ball_x << data.ball_y;
}

Packet& operator>>(sf::Packet& packet, NetworkPayload& data) {
  return packet >> data.ball_x >> data.ball_y;
}