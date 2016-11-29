#ifndef DISPLAY_H
#define DISPLAY_H

#include <MI0283QT9.h>

#define LCD_WIDTH (320)
#define LCD_HEIGHT (240)
#define BUTTON_HEIGHT (60)
#define BUTTON_WIDTH (85)
#define BUTTON_HEIGHT_OFFSET (LCD_HEIGHT - BUTTON_HEIGHT - 20)

#define INTRO (0)
#define PLAY (1)
#define SETUP (2)

class Display {
public:
  Display();
  void writeCalibrationData();
  uint8_t readCalibrationData();
  MI0283QT9 getLcd();
  int getScreen();
  void drawButtonBg(unsigned int left, bool pressed);
  void drawPlayPause(bool pressed);
  void drawNextButton(bool pressed);
  void drawPreviousButton(bool pressed);
  void drawSetupButton(unsigned int top, bool pressed);
  void drawSetupMenu(int items);
  void drawIntro();
  void drawPlayMenu();
  void setupLcd();
  void update();
  void (*onPreviousClick)();
  void (*onPlayPauseClick)();
  void (*onNextClick)();
  void (*onMenuOptionClick)(unsigned int); // Params: option index (0, 1, ...)
private:
  MI0283QT9 lcd;
  int screen;
  int lastScreen;
  bool touching = false;
  unsigned long lastTouched = 0;
};

#endif
