#include <GraphicsLib.h>
#include <MI0283QT9.h>
#include <EEPROM.h>

MI0283QT9 lcd;  //MI0283QT9 Adapter v1
bool touching = false;

uint16_t drawcolor[] = {
  RGB( 15, 15, 15), //bg
  RGB(255,  0, 0), // previous
  RGB(0, 255, 0), // play
  RGB(0, 0, 255), // next
  RGB(255, 255, 255)
};

#define BG_COLOR (0)
#define RED (1)
#define GREEN (2)
#define BLUE (3)
#define WHITE (4)

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
  lcd.fillRoundRect(20, 170, 50, 50, 5, drawcolor[RED]);
  lcd.fillRoundRect(130, 170, 50, 50, 5, drawcolor[GREEN]);
  lcd.fillRoundRect(250, 170, 50, 50, 5, drawcolor[BLUE]);
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
          Serial.println("Play");
          state = "Play";
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
