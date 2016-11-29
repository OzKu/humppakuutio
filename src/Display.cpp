#include "Display.h"

#include <EEPROM.h>
#include "colors.h"

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
  const int color = pressed ? BUTTON_PRESSED_BG : BUTTON_BG;
  lcd.fillRect(left, BUTTON_HEIGHT_OFFSET, BUTTON_WIDTH, BUTTON_HEIGHT, drawcolor[color]);
}

void Display::drawPlayPause(bool paused, bool pressed) {
  if(screen == PLAY){
  drawButtonBg(117, pressed);
    if (!paused) {
      lcd.fillRect(141, 170, 15, 40, drawcolor[BG_COLOR]);
      lcd.fillRect(162, 170, 15, 40, drawcolor[BG_COLOR]);
    } else {
      lcd.fillTriangle(145, 170, 145, 210, 175, 190, drawcolor[BG_COLOR]);
    }
  }
}

void Display::drawNextButton(bool pressed) {
  drawButtonBg(215, pressed);
  lcd.fillTriangle(236, 170, 236, 210, 266, 190, drawcolor[BG_COLOR]);
  lcd.fillRect(271, 170, 10, 40, drawcolor[BG_COLOR]);
}

void Display::drawPreviousButton(bool pressed) {
  drawButtonBg(20, pressed);
  lcd.fillTriangle(56, 190, 86, 210, 86, 170, drawcolor[BG_COLOR]);
  lcd.fillRect(41, 170, 10, 40, drawcolor[BG_COLOR]);
}

void Display::drawSetupButton(unsigned int top, bool pressed, unsigned int pageMode, uint8_t iwrap_autocall_target){
  int color;
  if (top == pageMode) color=BUTTON_ENABLED;
  else color = pressed ? BUTTON_PRESSED_BG : BUTTON_BG;
  lcd.fillRect(10, 10+(top*60), 300, 50, drawcolor[color]);
  switch (top) {
    case 0:
      lcd.drawText(50, 35, "Discoverable and connectable", drawcolor[WHITE], drawcolor[color], 1);
    case 1:
      lcd.drawText(45, 95, "Undiscoverable but connectable", drawcolor[WHITE], drawcolor[color], 1);
    case 2:
      lcd.drawText(35, 155, "Undiscoverable and unconnectable", drawcolor[WHITE], drawcolor[color], 1);
    case 3:
      if (iwrap_autocall_target) lcd.drawText(95, 215, "Disable auto-call", drawcolor[WHITE], drawcolor[color], 1);
      else lcd.drawText(100, 215, "Enable auto-call", drawcolor[WHITE], drawcolor[color], 1);
  }
}

void Display::drawSetupMenu(int items){
  lcd.fillScreen(drawcolor[BG_COLOR]);
  for(int i=0;i<items;i++){
    drawSetupButton(i,false);
  }
  this->lastScreen = this->screen;
  this->screen = SETUP;
}

void Display::drawIntro(){
  lcd.fillScreen(drawcolor[BG_COLOR]);
  lcd.drawText(20, 60, "HUMPPA", drawcolor[GREEN], drawcolor[BG_COLOR], 3);
  lcd.drawText(20, 120, "KUUTIO", drawcolor[GREEN], drawcolor[BG_COLOR], 3);
  this->lastScreen = this->screen;
  screen=INTRO;
  lastIntro = millis();
}

void Display::drawPlayMenu(){
  this->lastScreen = this->screen;
  screen=PLAY;
  lcd.fillScreen(drawcolor[BG_COLOR]);
  drawPlayPause(false);
  drawNextButton(false);
  drawPreviousButton(false);
  drawArtistTitle();
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
  // drawArtistTitle();

  static int pressedButton = -1;
  if (lcd.touchZ()) {
    lastTouched = millis();
    if (!touching) {
      unsigned short x = lcd.touchX();
      unsigned short y = lcd.touchY();
      String state = "";

      //Different handlings for touches on different screens.
      switch (screen) {
        case PLAY:
          if (y >= 170) {
            if (x <= 105) {
              // Prev
              Serial.println("Previous");
              state = "Previous";
              pressedButton = 0;
              display.drawPreviousButton(true);
              iwrap_send_command("AVRCP BACKWARD", IWRAP_MODE_MUX);
              //iwrap_send_command("AVRCP PDU 20 2 1 2", IWRAP_MODE_MUX);
            } else if (x >= 117 && x <= 202) {
              // Play
              if (paused) {
                Serial.println("Play");
                state = "Play";
                iwrap_send_command("AVRCP PLAY", IWRAP_MODE_MUX);
              } else {
                Serial.println("Pause");
                state = "Pause";
                iwrap_send_command("AVRCP PAUSE", IWRAP_MODE_MUX);
              }
              paused = !paused;
              pressedButton = 1;
              display.drawPlayPause(true);
            } else if (x >= 215) {
              // Next
              Serial.println("Next");
              state = "Next";
              pressedButton = 2;
              display.drawNextButton(true);
              iwrap_send_command("AVRCP FORWARD", IWRAP_MODE_MUX);
            }
          }
          else if(x > 280 && y < 40) display.drawSetupMenu(4);
          else iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);
          break;

        case INTRO:
          if (x > 280 && y > 120 && y < 140) display.drawSetupMenu(4);
          break;
        case SETUP:

          unsigned int old_PM = pageMode;
          if (y <= 65) {
            //Discoverable and connectable
            pageMode = 0;
            display.drawSetupButton(old_PM,false);
            display.drawSetupButton(0,true);
            pressedButton=0;
            iwrap_send_command("SET BT PAGEMODE 3", iwrap_mode);
          } else if (y>65 && y<=125) {
            //Undiscoverable but connectable
            pageMode = 1;
            display.drawSetupButton(old_PM,false);
            display.drawSetupButton(1,true);
            pressedButton=1;
            iwrap_send_command("SET BT PAGEMODE 2", iwrap_mode);
          } else if (y>125 && y<=185) {
            //Undiscoverable and unconnectable
            pageMode = 2;
            display.drawSetupButton(old_PM,false);
            display.drawSetupButton(2,true);
            pressedButton=2;
            iwrap_send_command("SET BT PAGEMODE 0", iwrap_mode);
          } else if (y>185) {
            Serial.println("menu 4");
            display.drawSetupButton(3,true);
            pressedButton=3;
            if (iwrap_autocall_target) {
                serial_out("=> (1) Disabling round-robin autocall algorithm\n\n");
                iwrap_autocall_target = 0;
            } else {
                serial_out("=> (1) Enabling round-robin autocall algorithm\n\n");
                iwrap_autocall_target = 1;
            }
          }

          break;
    }

     // lcd.fillRect(0, 0, 320, 30, drawcolor[BG_COLOR]);
     // lcd.drawText(20, 20, state, drawcolor[WHITE], drawcolor[BG_COLOR], 2);

    }
    touching = true;
  } else {
    touching = false;
    switch(screen) {
      case PLAY:
        switch (pressedButton) {
          case 0:
            display.drawPreviousButton(false);
            break;
          case 1:
            display.drawPlayPause(false);
            break;
          case 2:
            display.drawNextButton(false);
            break;
          }
        pressedButton = -1;
        break;
      case SETUP:
        switch (pressedButton) {
          case 0:
            display.drawSetupButton(0,false);
            break;
          case 1:
            display.drawSetupButton(1,false);
            break;
          case 2:
            display.drawSetupButton(2,false);
            break;
          case 3:
            display.drawSetupButton(3,false);
            break;
          }
        pressedButton = -1;
        break;
      }
      if (millis()-lastTouched > 5000 && screen == SETUP){
        switch (last_screen) {
          case INTRO:
            display.drawIntro();
            break;
          case PLAY:
            display.drawPlayMenu();
            break;
        }
      }

    }
}
