#include "NetworkPayload.h"

Packet &operator<<(sf::Packet &packet, const NetworkPayload &data) {
  return packet
         << data.ball_x
         << data.ball_y
         << data.player1_score
         << data.player2_score
         << data.host_y
         << data.player_owning_ball;
}

Packet &operator>>(sf::Packet &packet, NetworkPayload &data) {
  return packet >> data.ball_x >> data.ball_y >> data.player1_score >> data.player2_score >> data.host_y >> data.player_owning_ball;
}

Packet NetworkPayload::pack() {
  Packet packet;
  packet << *this;
  return packet;
}

void NetworkPayload::unpack(Packet *packet) {
  *packet >> *this;
}