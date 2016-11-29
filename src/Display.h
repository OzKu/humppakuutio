#pragma once

#include <MI0283QT9.h>

#define BG_COLOR (0)
#define RED (1)
#define GREEN (2)
#define BLUE (3)
#define WHITE (4)
#define BUTTON_BG (5)
#define BUTTON_PRESSED_BG (6)
#define BUTTON_ENABLED (7)

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
  void drawPlayPause(bool paused, bool pressed);
  void drawNextButton(bool pressed);
  void drawPreviousButton(bool pressed);
  void drawSetupButton(unsigned int top, bool pressed, unsigned int pageMode, uint8_t iwrap_autocall_target);
  void drawSetupMenu(int items);
  void drawIntro();
  void drawPlayMenu();
  void setupLcd();
  void update();
  void (*onPreviousClick)();
  void (*onPlayPauseClick)();
  void (*onNextClick)();
private:
  MI0283QT9 lcd;
  int screen;
  int lastScreen;
};
