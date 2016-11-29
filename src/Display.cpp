#include "Display.h"

#include <EEPROM.h>
#include <Arduino.h>

#include "colors.h"
#include "State.h"

Display::Display() {
  this->screen = INTRO;
  this->lastScreen = this->screen;
}

void Display::writeCalibrationData()
{
  uint16_t i, addr=0;
  uint8_t *ptr;

  EEPROM.write(addr++, 0xAA);

  ptr = (uint8_t*)&lcd.tp_matrix;
  for(i=0; i < sizeof(CAL_MATRIX); i++)
  {
    EEPROM.write(addr++, *ptr++);
  }

  return;
}


uint8_t Display::readCalibrationData()
{
  uint16_t i, addr=0;
  uint8_t *ptr;
  uint8_t c;

  c = EEPROM.read(addr++);
  if(c == 0xAA)
  {
    ptr = (uint8_t*)&lcd.tp_matrix;
    for(i=0; i < sizeof(CAL_MATRIX); i++)
    {
      *ptr++ = EEPROM.read(addr++);
    }
    return 0;
  }

  return 1;
}

MI0283QT9 Display::getLcd() {
  return this->lcd;
}

int Display::getScreen() {
  return this->screen;
}

void Display::drawButtonBg(unsigned int left, bool pressed) {
  const int color = pressed ? COLOR_BUTTON_PRESSED_BG : COLOR_BUTTON_BG;
  lcd.fillRect(left, BUTTON_HEIGHT_OFFSET, BUTTON_WIDTH, BUTTON_HEIGHT, color);
}

void Display::drawPlayPause(bool pressed) {
  if(screen == PLAY){
  drawButtonBg(117, pressed);
    if (!State::getInstance()->paused) {
      lcd.fillRect(141, 170, 15, 40, COLOR_BG);
      lcd.fillRect(162, 170, 15, 40, COLOR_BG);
    } else {
      lcd.fillTriangle(145, 170, 145, 210, 175, 190, COLOR_BG);
    }
  }
}

void Display::drawNextButton(bool pressed) {
  drawButtonBg(215, pressed);
  lcd.fillTriangle(236, 170, 236, 210, 266, 190, COLOR_BG);
  lcd.fillRect(271, 170, 10, 40, COLOR_BG);
}

void Display::drawPreviousButton(bool pressed) {
  drawButtonBg(20, pressed);
  lcd.fillTriangle(56, 190, 86, 210, 86, 170, COLOR_BG);
  lcd.fillRect(41, 170, 10, 40, COLOR_BG);
}

void Display::drawSetupButton(unsigned int top, bool pressed){
  int color;
  if (top == State::getInstance()->pageMode) color = COLOR_BUTTON_ENABLED;
  else color = pressed ? COLOR_BUTTON_PRESSED_BG : COLOR_BUTTON_BG;
  lcd.fillRect(10, 10+(top*60), 300, 50, color);
  switch (top) {
    case 0:
      lcd.drawText(50, 35, "Discoverable and connectable", COLOR_WHITE, color, 1);
    case 1:
      lcd.drawText(45, 95, "Undiscoverable but connectable", COLOR_WHITE, color, 1);
    case 2:
      lcd.drawText(35, 155, "Undiscoverable and unconnectable", COLOR_WHITE, color, 1);
    case 3:
      if (State::getInstance()->iwrap_autocall_target) lcd.drawText(95, 215, "Disable auto-call", COLOR_WHITE, color, 1);
      else lcd.drawText(100, 215, "Enable auto-call", COLOR_WHITE, color, 1);
  }
}

void Display::drawSetupMenu(int items){
  lcd.fillScreen(COLOR_BG);
  for(int i=0;i<items;i++){
    drawSetupButton(i,false);
  }
  this->lastScreen = this->screen;
  this->screen = SETUP;
}

void Display::drawIntro(){
  lcd.fillScreen(COLOR_BG);
  lcd.drawText(20, 60, "HUMPPA", COLOR_GREEN, COLOR_BG, 3);
  lcd.drawText(20, 120, "KUUTIO", COLOR_GREEN, COLOR_BG, 3);
  this->lastScreen = this->screen;
  screen=INTRO;
  State::getInstance()->lastIntro = millis();
}

void Display::drawPlayMenu(){
  this->lastScreen = this->screen;
  screen=PLAY;
  lcd.fillScreen(COLOR_BG);
  drawPlayPause(false);
  drawNextButton(false);
  drawPreviousButton(false);
}

void Display::setupLcd() {
  lcd.begin();
  lcd.led(100);
  lcd.touchRead();
  if(lcd.touchZ() || readCalibrationData()) //calibration data in EEPROM?
  {
    lcd.touchStartCal(); //calibrate touchpanel
    writeCalibrationData(); //write data to EEPROM
  }
}

void Display::update() {
  lcd.touchRead();

  static int pressedButton = -1;
  if (lcd.touchZ()) {
    this->lastTouched = millis();
    if (!this->touching) {
      unsigned short x = lcd.touchX();
      unsigned short y = lcd.touchY();

      //Different handlings for touches on different screens.
      switch (screen) {
        case PLAY:
          if (y >= 170) {
            if (x <= 105) {
              if (this->onPreviousClick) this->onPreviousClick();
              pressedButton = 0;
            } else if (x >= 117 && x <= 202) {
              if (this->onPlayPauseClick) this->onPlayPauseClick();
              pressedButton = 1;
            } else if (x >= 215) {
              if (this->onNextClick) this->onNextClick();
              pressedButton = 2;
            }
          }
          else if(x > 280 && y < 40) this->drawSetupMenu(4);
          // else iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);
          break;

        case INTRO:
          if (x > 280 && y > 120 && y < 140) this->drawSetupMenu(4);
          break;
        case SETUP:

          // TODO: Move application logic outside this class, only display / touch input related
          // logic belongs here
          unsigned int old_PM = State::getInstance()->pageMode;
          if (y <= 65) {
            //Discoverable and connectable
            this->drawSetupButton(old_PM,false);
            this->drawSetupButton(0,true);
            pressedButton=0;
            if (this->onMenuOptionClick) this->onMenuOptionClick(0);
          } else if (y>65 && y<=125) {
            //Undiscoverable but connectable
            this->drawSetupButton(old_PM,false);
            this->drawSetupButton(1,true);
            pressedButton=1;
            if (this->onMenuOptionClick) this->onMenuOptionClick(1);
          } else if (y>125 && y<=185) {
            //Undiscoverable and unconnectable
            this->drawSetupButton(old_PM,false);
            this->drawSetupButton(2,true);
            pressedButton=2;
            if (this->onMenuOptionClick) this->onMenuOptionClick(2);
          } else if (y>185) {
            this->drawSetupButton(3,true);
            pressedButton=3;
            if (this->onMenuOptionClick) this->onMenuOptionClick(3);
          }

          break;
        }
    }
    this->touching = true;
  } else {
    this->touching = false;
    switch(screen) {
      case PLAY:
        switch (pressedButton) {
          case 0:
            this->drawPreviousButton(false);
            break;
          case 1:
            this->drawPlayPause(false);
            break;
          case 2:
            this->drawNextButton(false);
            break;
          }
        pressedButton = -1;
        break;
      case SETUP:
        switch (pressedButton) {
          case 0:
            this->drawSetupButton(0,false);
            break;
          case 1:
            this->drawSetupButton(1,false);
            break;
          case 2:
            this->drawSetupButton(2,false);
            break;
          case 3:
            this->drawSetupButton(3,false);
            break;
          }
        pressedButton = -1;
        break;
      }
      if (millis()-this->lastTouched > 5000 && screen == SETUP){
        switch (this->lastScreen) {
          case INTRO:
            this->drawIntro();
            break;
          case PLAY:
            this->drawPlayMenu();
            break;
        }
      }

    }
}
