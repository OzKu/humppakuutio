#include <GraphicsLib.h>
#include <MI0283QT9.h>
#include <EEPROM.h>

MI0283QT9 lcd;  //MI0283QT9 Adapter v1
bool touching = false;
bool paused = false;

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
#define BUTTON_BORDER_RADIUS (8)
#define BUTTON_HEIGHT_OFFSET (LCD_HEIGHT - BUTTON_HEIGHT - 20)

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


void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  lcd.begin();
  lcd.led(100);
  lcd.touchRead();
  if(lcd.touchZ() || readCalData()) //calibration data in EEPROM?
  {
    lcd.touchStartCal(); //calibrate touchpanel
    writeCalData(); //write data to EEPROM
  }

  lcd.fillScreen(drawcolor[BG_COLOR]);
  drawPlayPause();
  drawNextButton();
  drawPreviousButton();
}

void drawButtonBg(unsigned int left, bool pressed) {
  const int color = pressed ? BUTTON_PRESSED_BG : BUTTON_BG;
  lcd.fillRoundRect(left, BUTTON_HEIGHT_OFFSET, BUTTON_WIDTH, BUTTON_HEIGHT, BUTTON_BORDER_RADIUS, drawcolor[color]);
}

void drawPlayPause() {
  drawButtonBg(117, false);
  if (paused) {
    lcd.fillRect(141, 170, 15, 40, drawcolor[BG_COLOR]);
    lcd.fillRect(162, 170, 15, 40, drawcolor[BG_COLOR]);
  } else {
    lcd.fillTriangle(145, 170, 145, 210, 175, 190, drawcolor[BG_COLOR]);
  }
}

void drawNextButton() {
  drawButtonBg(215, false);
  lcd.fillTriangle(236, 170, 236, 210, 266, 190, drawcolor[BG_COLOR]);
  lcd.fillRect(271, 170, 10, 40, drawcolor[BG_COLOR]);
}

void drawPreviousButton() {
  drawButtonBg(20, false);
  lcd.fillTriangle(56, 190, 86, 210, 86, 170, drawcolor[BG_COLOR]);
  lcd.fillRect(41, 170, 10, 40, drawcolor[BG_COLOR]);
}

void loop() {
  lcd.touchRead();

  if (lcd.touchZ()) {
    if (!touching) {
      unsigned short x = lcd.touchX();
      unsigned short y = lcd.touchY();
      String state = "";
  
      if (y >= 170 && y <= 220) {
        if (x >= 20 && x <= 70) {
          // Prev
          Serial.println("Previous");
          state = "Previous";
        } else if (x >= 130 && x <= 180) {
          // Play
          if (paused) {
            Serial.println("Play");
            state = "Play";
          } else {
            Serial.println("Pause");
            state = "Pause";
          }
          paused = !paused;
          drawPlayPause();
        } else if (x >= 250 && x <= 300) {
          // Next
          Serial.println("Next");
          state = "Next";
        }
      }

      lcd.fillRect(20, 20, 280, 30, drawcolor[BG_COLOR]);
      lcd.drawText(20, 20, state, drawcolor[WHITE], drawcolor[BG_COLOR], 2);
    }
    touching = true;
  } else {
    touching = false;
  }
}
