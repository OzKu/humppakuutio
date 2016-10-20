#pragma once

#include <Arduino.h>
#include <MI0283QT9.h>

class ScrollingText {
private:
  String text;
  unsigned short x = 0;
  unsigned short y = 0;
  unsigned interval = 250;
  unsigned long lastUpdate;
  unsigned startIndex = 0;
  unsigned displayChars = 19;
  MI0283QT9 lcd;
  int color;
  int backgroundColor;
  
public:
  ScrollingText(MI0283QT9 lcd);
  void setText(String text);
  void setPosition(unsigned short x, unsigned short y);
  void update();
  void setColor(int color);
  void setBackgroundColor(int color);
};

