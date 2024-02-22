#ifndef PONG_POINTCOUNTER_H
#define PONG_POINTCOUNTER_H

#include "../GameHandler/GameHandler.h"
#include "../EntityHandler/EntityHandler.h"
#include "../Player/Player.h"
#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

class PointCounter {
public:
  PointCounter();

  void draw();

  void increment(int index);

  vector<int> getScores();

  void setScoresFromParams(int player1, int player2);

private:
  RenderWindow *window;
  Font *font;
  vector<Player*> *players;
  vector<int> scores = vector<int>(6, 7);
};


#endif//PONG_POINTCOUNTER_H