#include <GraphicsLib.h>
#include <MI0283QT9.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <iWRAP.h>

#include "ScrollingText.h"

MI0283QT9 lcd;  //MI0283QT9 Adapter v1
bool touching = false;
bool paused = false;
ScrollingText *text;
SoftwareSerial wtSerial(10, 2);

uint16_t drawcolor[] = {
  RGB( 15, 15, 15), //bg
  RGB(255,  0, 0), // previous
  RGB(0, 255, 0), // play
  RGB(0, 0, 255), // next
  RGB(255, 255, 255), // white
  RGB(130, 130, 130), // button bg
  RGB(110, 110, 110), // button pressed bg
};

#define BG_COLOR (0)
#define RED (1)
#define GREEN (2)
#define BLUE (3)
#define WHITE (4)
#define BUTTON_BG (5)
#define BUTTON_PRESSED_BG (6)

#define LCD_WIDTH (320)
#define LCD_HEIGHT (240)
#define BUTTON_HEIGHT (60)
#define BUTTON_WIDTH (85)
#define BUTTON_HEIGHT_OFFSET (LCD_HEIGHT - BUTTON_HEIGHT - 20)

int iwrap_out(int len, unsigned char *data) {
  return wtSerial.write(data, len);
}

void writeCalData(void)
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


uint8_t readCalData(void)
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

int serial_out(const char *str) {
  // debug output to host goes through hardware serial
  return Serial.print(str);
}


void setup() {
  Serial.begin(9600);
  while (!Serial);

  wtSerial.begin(38400);
  iwrap_output = iwrap_out;
  iwrap_debug = serial_out;
  
  lcd.begin();
  lcd.led(100);
  lcd.touchRead();
  if(lcd.touchZ() || readCalData()) //calibration data in EEPROM?
  {
    lcd.touchStartCal(); //calibrate touchpanel
    writeCalData(); //write data to EEPROM
  }

  lcd.fillScreen(drawcolor[BG_COLOR]);
  drawPlayPause(false);
  drawNextButton(false);
  drawPreviousButton(false);

  text = new ScrollingText(lcd);
  text->setText("0 to 100 / The Catch Up - Drake");
  text->setPosition(10, 10);
  text->setColor(drawcolor[WHITE]);
  text->setBackgroundColor(drawcolor[BG_COLOR]);
}

void drawButtonBg(unsigned int left, bool pressed) {
  const int color = pressed ? BUTTON_PRESSED_BG : BUTTON_BG;
  lcd.fillRect(left, BUTTON_HEIGHT_OFFSET, BUTTON_WIDTH, BUTTON_HEIGHT, drawcolor[color]);
}

void drawPlayPause(bool pressed) {
  drawButtonBg(117, pressed);
  if (paused) {
    lcd.fillRect(141, 170, 15, 40, drawcolor[BG_COLOR]);
    lcd.fillRect(162, 170, 15, 40, drawcolor[BG_COLOR]);
  } else {
    lcd.fillTriangle(145, 170, 145, 210, 175, 190, drawcolor[BG_COLOR]);
  }
}

void drawNextButton(bool pressed) {
  drawButtonBg(215, pressed);
  lcd.fillTriangle(236, 170, 236, 210, 266, 190, drawcolor[BG_COLOR]);
  lcd.fillRect(271, 170, 10, 40, drawcolor[BG_COLOR]);
}

void drawPreviousButton(bool pressed) {
  drawButtonBg(20, pressed);
  lcd.fillTriangle(56, 190, 86, 210, 86, 170, drawcolor[BG_COLOR]);
  lcd.fillRect(41, 170, 10, 40, drawcolor[BG_COLOR]);
}

void loop() {
  lcd.touchRead();
  text->update();

  uint16_t result;
  if ((result = wtSerial.read()) < 256) {
    iwrap_parse(result & 0xFF, IWRAP_MODE_MUX);
  }

  static int pressedButton = -1;
  if (lcd.touchZ()) {
    if (!touching) {
      unsigned short x = lcd.touchX();
      unsigned short y = lcd.touchY();
      String state = "";
  
      if (y >= 170) {
        if (x <= 105) {
          // Prev
          Serial.println("Previous");
          state = "Previous";
          pressedButton = 0;
          drawPreviousButton(true);
          iwrap_send_command("AVRCP BACKWARD", IWRAP_MODE_MUX);
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
          drawPlayPause(true);
        } else if (x >= 215) {
          // Next
          Serial.println("Next");
          state = "Next";
          pressedButton = 2;
          drawNextButton(true);
          iwrap_send_command("AVRCP PDU 30", IWRAP_MODE_MUX);
        }
      }

      lcd.fillRect(20, 20, 280, 30, drawcolor[BG_COLOR]);
      lcd.drawText(20, 20, state, drawcolor[WHITE], drawcolor[BG_COLOR], 2);
    }
    touching = true;
  } else {
    touching = false;
    switch (pressedButton) {
      case 0: 
        drawPreviousButton(false);
        break;
      case 1: 
        drawPlayPause(false);
        break;
      case 2: 
        drawNextButton(false);
        break;
    }
    pressedButton = -1;
  }
}
