#ifndef PONG_MENUHANDLER_H
#define PONG_MENUHANDLER_H

#include "SFML/Graphics.hpp"

class GameHandler;

using namespace sf;

class MenuHandler {
public:
  MenuHandler();
  void drawMenu(Event *event);
  void drawCreateOnline(Event *event);
  void drawConnectOnline(Event *event);

  void drawConnectOnlineInput(Event *event);

  void updateIpText(Event *event);

private:
  Font font;
  Text title;
  Text playLocal;
  Text createOnline;
  Text playOnline;
  Text quit;
  RectangleShape button;
  Text buttonText;

  RectangleShape inputBox;

  String userIp;
  Text userIpText;

  RenderWindow *window;
  void handleDefaultEvent(Event *event, GameHandler *gameHandler);

  float windowCenterX;
};


#endif//PONG_MENUHANDLER_H
