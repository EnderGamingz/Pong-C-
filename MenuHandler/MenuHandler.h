#ifndef PONG_MENUHANDLER_H
#define PONG_MENUHANDLER_H

#include "SFML/Graphics.hpp"

using namespace sf;

class MenuHandler {
public:
  MenuHandler();
  void drawMenu(Event *event);
  void drawCreateOnline(Event *event);
  void drawConnectOnline(Event *event);

private:
  Font font;
  Text title;
  Text playLocal;
  Text createOnline;
  Text playOnline;
  Text quit;
  Text testButton;

  RenderWindow *window;
};


#endif//PONG_MENUHANDLER_H
