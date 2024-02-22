#include "ClientToHostPayload.h"

Packet &operator<<(sf::Packet &packet, const ClientToHostPayload &data) {
  return packet
         << data.client_y;
}

Packet &operator>>(sf::Packet &packet, ClientToHostPayload &data) {
  return packet >> data.client_y;
}

Packet ClientToHostPayload::pack() {
  Packet packet;
  packet << *this;
  return packet;
}

void ClientToHostPayload::unpack(Packet *packet) {
  *packet >> *this;
}