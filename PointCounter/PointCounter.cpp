#include "PointCounter.h"
#include <iostream>

PointCounter::PointCounter() {
  this->window = GameHandler::getInstance().getWindow();
  this->players = EntityHandler::getInstance().getPlayers();
  this->font = &GameHandler::getInstance().getFont();
  scores = vector<int>(players->size(), 0);
}

void PointCounter::draw() {
  for (int i = 0; i < players->size(); i++) {
    auto &player = players->at(i);
    Text text(to_string(scores[i]), *font, 20);
    text.setFillColor(Color::White);
    text.setOutlineColor(Color::Black);
    text.setOutlineThickness(1.0f);
    text.setStyle(Text::Bold);
    switch (player->getSide()) {
      case PlayerSide::LEFT:
        text.setPosition(player->playerOffset, 0);
        break;
      case PlayerSide::RIGHT:
        text.setPosition(GameHandler::getInstance().window_width - 110 - player->playerOffset, 0);
        break;
    }
    text.setString("Player " + to_string(i + 1) + ": " + to_string(scores[i]));
    window->draw(text);
  }
}

void PointCounter::increment(int index) {
  if (index >= 0 && index < players->size()) {
    scores[index]++;
  } else {
    cout << "Invalid player index" << endl;
  }
}

vector<int> PointCounter::getScores() {
  return scores;
}

void PointCounter::setScoresFromParams(int player1, int player2) {
  scores[0] = player1;
  scores[1] = player2;
}
