#define PLATFORM_ARDUINO_UNO
#define HOST_BAUD                   38400   // works with 8MHz CPU clock
#define IWRAP_BAUD                  38400   // works with 8MHz CPU clock (REQUIRES iWRAP RECONFIGURATION, DEFAULT IS 115200)
#define MODULE_RESET_PIN            12      // optional connection for MCU-based module reset

#define IWRAP_DEBUG
#define IWRAP_INCLUDE_RSP_CALL
#define IWRAP_INCLUDE_RSP_LIST_COUNT
#define IWRAP_INCLUDE_RSP_LIST_RESULT
#define IWRAP_INCLUDE_RSP_SET
#define IWRAP_INCLUDE_EVT_CONNECT
#define IWRAP_INCLUDE_EVT_NO_CARRIER
#define IWRAP_INCLUDE_EVT_PAIR
#define IWRAP_INCLUDE_EVT_READY
#define IWRAP_INCLUDE_EVT_RING
#define IWRAP_INCLUDE_EVT_A2DP_STREAMING_START      // READY
#define IWRAP_INCLUDE_EVT_A2DP_STREAMING_STOP       // READY
#define IWRAP_CONFIGURED

#define LCD

#include <GraphicsLib.h>
#include <MI0283QT9.h>
#include <EEPROM.h>
#include <Arduino.h>
// #include <SoftwareSerial.h>
#include "iWRAP.h"

#include "ScrollingText.h"

MI0283QT9 lcd;  //MI0283QT9 Adapter v1
bool touching = false;
bool paused = false;
ScrollingText *text;
// SoftwareSerial Serial1(10, 2);
unsigned long textChanged = 0;

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


// IWRAP
#define IWRAP_STATE_IDLE            0
#define IWRAP_STATE_UNKNOWN         1
#define IWRAP_STATE_PENDING_AT      2
#define IWRAP_STATE_PENDING_SET     3
#define IWRAP_STATE_PENDING_LIST    4
#define IWRAP_STATE_PENDING_CALL    5
#define IWRAP_STATE_COMM_FAILED     255

#define IWRAP_MAX_PAIRINGS          16

String global_title = "Title";
String prev_title = " ";
String global_artist = "Artist";
String prev_artist = " ";

// connection map structure
typedef struct {
    iwrap_address_t mac;
    uint8_t active_links;
    uint8_t link_a2dp1;
    uint8_t link_a2dp2;
    uint8_t link_avrcp;
    uint8_t link_hfp;
    uint8_t link_hfpag;
    uint8_t link_hid_control;
    uint8_t link_hid_interrupt;
    uint8_t link_hsp;
    uint8_t link_hspag;
    uint8_t link_iap;
    uint8_t link_sco;
    uint8_t link_spp;
    // other profile-specific link IDs may be added here
} iwrap_connection_t;
iwrap_connection_t *iwrap_connection_map[IWRAP_MAX_PAIRINGS];

// iwrap state tracking info
uint8_t iwrap_mode = IWRAP_MODE_MUX;
uint8_t iwrap_state = IWRAP_STATE_UNKNOWN;
uint8_t iwrap_initialized = 0;
uint32_t iwrap_time_ref = 0;
uint8_t iwrap_pairings = 0;
uint8_t iwrap_pending_calls = 0;
uint8_t iwrap_pending_call_link_id = 0xFF;
uint8_t iwrap_connected_devices = 0;
uint8_t iwrap_active_connections = 0;
uint8_t iwrap_autocall_target = 0;
uint16_t iwrap_autocall_delay_ms = 10000;
uint32_t iwrap_autocall_last_time = 0;
uint8_t iwrap_autocall_index = 0;

// general helper functions
uint8_t find_pairing_from_mac(const iwrap_address_t *mac);
uint8_t find_pairing_from_link_id(uint8_t link_id);
void add_mapped_connection(uint8_t link_id, const iwrap_address_t *addr, const char *mode, uint16_t channel);
uint8_t remove_mapped_connection(uint8_t link_id);
void print_connection_map();
void print_demo_menu();
void process_demo_choice(char b);

// platform-specific helper functions
int serial_out(const char *str);
int serial_out(const __FlashStringHelper *str);
int iwrap_out(int len, unsigned char *data);
// IWRAP

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

void my_iwrap_evt_a2dp_streaming_start(uint8_t link_id) {
  Serial.println("!! streaming start");
}

void my_iwrap_evt_a2dp_streaming_stop(uint8_t link_id) {
  Serial.println("!! streaming stop");
}

void my_iwrap_evt_track_changed() {
  Serial.println("!! track changed");
  iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);  // request track change
  iwrap_send_command("AVRCP PDU 20 1 1", IWRAP_MODE_MUX);  // request title
  iwrap_send_command("AVRCP PDU 20 1 2", IWRAP_MODE_MUX);  // request artist
}

void my_iwrap_evt_title_received(const char *rawTitle) {
  String title = String(rawTitle);
  title = title.substring(48, title.length() - 3);
  global_title = title;
  Serial.println("title: " + title);
  textChanged = millis();
}

void my_iwrap_evt_artist_received(const char *rawArtist) {
  String artist = String(rawArtist);
  artist = artist.substring(49, artist.length() - 3);
  Serial.println("artisti makssaaaaa: '" + artist + "'");
  textChanged = millis();
  global_artist = artist;
}

/*void setup() {
  Serial.begin(38400);
  while (!Serial);
  Serial1.begin(38400);

  // assign transport/debug output
  iwrap_output = iwrap_out;
  #ifdef IWRAP_DEBUG
      iwrap_debug = serial_out;
  #endif // IWRAP_DEBUG

  digitalWrite(MODULE_RESET_PIN, LOW);
  pinMode(MODULE_RESET_PIN, OUTPUT);
  
  //lcd.begin();
  //lcd.led(100);
  // lcd.touchRead();
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
}*/

void setup() {
  Serial.begin(HOST_BAUD);
  Serial1.begin(IWRAP_BAUD);

  digitalWrite(MODULE_RESET_PIN, LOW);
  pinMode(MODULE_RESET_PIN, OUTPUT);

  // assign transport/debug output
  iwrap_output = iwrap_out;
  #ifdef IWRAP_DEBUG
      iwrap_debug = serial_out;
  #endif // IWRAP_DEBUG

  serial_out(F("iWRAP host library generic demo started\n"));

  #ifdef LCD
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
  #endif

  iwrap_evt_a2dp_streaming_start = my_iwrap_evt_a2dp_streaming_start;
  iwrap_evt_a2dp_streaming_stop = my_iwrap_evt_a2dp_streaming_stop;
  iwrap_evt_track_changed = my_iwrap_evt_track_changed;
  iwrap_evt_title_received = my_iwrap_evt_title_received;
  iwrap_evt_artist_received = my_iwrap_evt_artist_received;
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

void loop2() {
  lcd.touchRead();
  // text->update();
  if(global_title != prev_title && millis() - textChanged > 50) {
    lcd.fillRect(0, 0, 320, 60, drawcolor[BG_COLOR]);
    lcd.drawText(20, 20, global_title, drawcolor[WHITE], drawcolor[BG_COLOR], 1);
    lcd.drawText(20, 40, global_artist, drawcolor[WHITE], drawcolor[BG_COLOR], 1);
    prev_title = global_title;
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
          drawPlayPause(true);
        } else if (x >= 215) {
          // Next
          Serial.println("Next");
          state = "Next";
          pressedButton = 2;
          drawNextButton(true);
          iwrap_send_command("AVRCP FORWARD", IWRAP_MODE_MUX);
        }
      }
      
     // lcd.fillRect(0, 0, 320, 30, drawcolor[BG_COLOR]);
     // lcd.drawText(20, 20, state, drawcolor[WHITE], drawcolor[BG_COLOR], 2);
      iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);
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

void loop() {
  #ifdef LCD
  loop2();
  #endif
  
    uint16_t result;

    // manage iWRAP state machine
    if (!iwrap_pending_commands) {
        // no pending commands, some state transition occurring
        if (iwrap_state) {
            // not idle, in the middle of some process
            if (iwrap_state == IWRAP_STATE_UNKNOWN) {
                // reset all detailed state trackers
                iwrap_initialized = 0;
                iwrap_pairings = 0;
                iwrap_pending_calls = 0;
                iwrap_pending_call_link_id = 0xFF;
                iwrap_connected_devices = 0;
                iwrap_active_connections = 0;
                
                // send command to test module connectivity
                serial_out(F("Testing iWRAP communication...\n"));
                iwrap_send_command("AT", iwrap_mode);
                iwrap_state = IWRAP_STATE_PENDING_AT;

                // initialize time reference for connectivity test timeout
                iwrap_time_ref = millis();
            } else if (iwrap_state == IWRAP_STATE_PENDING_AT) {
                // send command to dump all module settings and pairings
                serial_out(F("Getting iWRAP settings...\n"));
                iwrap_send_command("SET", iwrap_mode);
                iwrap_state = IWRAP_STATE_PENDING_SET;
            } else if (iwrap_state == IWRAP_STATE_PENDING_SET) {
                // send command to show all current connections
                serial_out(F("Getting active connection list...\n"));
                iwrap_send_command("LIST", iwrap_mode);
                iwrap_state = IWRAP_STATE_PENDING_LIST;
            } else if (iwrap_state == IWRAP_STATE_PENDING_LIST) {
                // all done!
                if (!iwrap_initialized) {
                    iwrap_initialized = 1;
                    serial_out(F("iWRAP initialization complete\n"));
                }
                print_connection_map();
                print_demo_menu();
                iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);
                iwrap_state = IWRAP_STATE_IDLE;
            } else if (iwrap_state == IWRAP_STATE_PENDING_CALL && !iwrap_pending_calls) {
                // all done!
                serial_out(F("Pending call processed\n"));
                iwrap_state = IWRAP_STATE_IDLE;
            }
        } else if (iwrap_initialized) {
            // idle
            if (iwrap_pairings && iwrap_autocall_target > iwrap_connected_devices && !iwrap_pending_calls
                               && (!iwrap_autocall_last_time || (millis() - iwrap_autocall_last_time) >= iwrap_autocall_delay_ms)) {
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 19 A2DP";      // A2DP
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 17 AVRCP";     // AVRCP
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 111F HFP";     // HFP
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 111E HFP-AG";  // HFP-AG
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 11 HID";       // HID
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 1112 HSP";     // HSP
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 1108 HSP-AG";  // HSP-AG
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF * IAP";        // IAP
                char cmd[] = "CALL AA:BB:CC:DD:EE:FF 1101 RFCOMM";  // SPP
                char *cptr = cmd + 5;
                
                // find first unconnected device
                for (; iwrap_connection_map[iwrap_autocall_index] -> active_links; iwrap_autocall_index++);
                
                // write MAC string into call command buffer and send it
                iwrap_bintohexstr((uint8_t *)(iwrap_connection_map[iwrap_autocall_index] -> mac.address), 6, &cptr, ':', 0);
                char s[21];
                sprintf(s, "Calling device #%d\r\n", iwrap_autocall_index);
                serial_out(s);
                iwrap_send_command(cmd, iwrap_mode);
                iwrap_autocall_last_time = millis();
            }
        }
    }
    
    // check for incoming iWRAP data
    #if defined(PLATFORM_ARDUINO_UNO)
        if ((result = Serial1.read()) < 256) iwrap_parse(result & 0xFF, iwrap_mode);
    #elif defined(PLATFORM_TEENSY2)
        if ((result = Serial1.read()) < 256) iwrap_parse(result & 0xFF, iwrap_mode);
    #endif
    
    // check for timeout if still testing communication
    if (!iwrap_initialized && iwrap_state == IWRAP_STATE_PENDING_AT) {
        if (millis() - iwrap_time_ref > 5000) {
            serial_out(F("ERROR: Could not communicate with iWRAP module\n"));
            iwrap_state = IWRAP_STATE_COMM_FAILED;
            iwrap_pending_commands = 0; // normally handled by the parser, but comms failed
        }
    }

    // check for incoming host data
    #if defined(PLATFORM_ARDUINO_UNO)
        if ((result = Serial.read()) < 256)
    #elif defined(PLATFORM_TEENSY2)
        if ((result = Serial.read()) < 256)
    #endif
    {
        process_demo_choice(result & 0xFF);
    }
}

uint8_t find_pairing_from_mac(const iwrap_address_t *mac) {
    uint8_t i;
    for (i = 0; i < iwrap_pairings; i++) {
        if (memcmp(&(iwrap_connection_map[i] -> mac), mac, sizeof(iwrap_address_t)) == 0) return i;
    }
    return i >= iwrap_pairings ? 0xFF : i;
}

uint8_t find_pairing_from_link_id(uint8_t link_id) {
    // NOTE: This implementation walks through the iwrap_connection_t struct memory
    // starting at the first link ID (A2DP #1) and going all the way to the end (if
    // necessary). This means it is contingent on the structure keeping this memory
    // arrangement, where the last set of whatever is contained there is always the
    // set of link IDs.
    
    uint8_t i, j, *idptr;
    for (i = 0; i < iwrap_pairings; i++) {
        idptr = &(iwrap_connection_map[i] -> link_a2dp1);
        for (j = 6; j < sizeof(iwrap_connection_t); j++, idptr++) if (idptr[0] == link_id) return i;
    }
    return 0xFF;
}

void add_mapped_connection(uint8_t link_id, const iwrap_address_t *addr, const char *mode, uint16_t channel) {
    uint8_t pairing_index = find_pairing_from_mac(addr);

    // make sure we found a match (we SHOULD always match something, if we properly parsed the pairing data first)
    if (pairing_index == 0xFF) return; // uh oh

    // updated connected device count and overall active link count for this device
    if (!iwrap_connection_map[pairing_index] -> active_links) iwrap_connected_devices++;
    iwrap_connection_map[pairing_index] -> active_links++;

    // add link ID to connection map
    if (strcmp(mode, "A2DP") == 0) {
        if (iwrap_connection_map[pairing_index] -> link_a2dp1 == 0xFF) {
            iwrap_connection_map[pairing_index] -> link_a2dp1 = link_id;
        } else {
            iwrap_connection_map[pairing_index] -> link_a2dp2 = link_id;
        }
    } else if (strcmp(mode, "AVRCP") == 0) {
        iwrap_connection_map[pairing_index] -> link_avrcp = link_id;
    } else if (strcmp(mode, "HFP") == 0) {
        iwrap_connection_map[pairing_index] -> link_hfp = link_id;
    } else if (strcmp(mode, "HFPAG") == 0) {
        iwrap_connection_map[pairing_index] -> link_hfpag = link_id;
    } else if (strcmp(mode, "HID") == 0) {
        if (channel == 0x11) {
            iwrap_connection_map[pairing_index] -> link_hid_control = link_id;
        } else {
            iwrap_connection_map[pairing_index] -> link_hid_interrupt = link_id;
        }
    } else if (strcmp(mode, "HSP") == 0) {
        iwrap_connection_map[pairing_index] -> link_hsp = link_id;
    } else if (strcmp(mode, "HSPAG") == 0) {
        iwrap_connection_map[pairing_index] -> link_hspag = link_id;
    } else if (strcmp(mode, "IAP") == 0) {
        iwrap_connection_map[pairing_index] -> link_iap = link_id;
    } else if (strcmp(mode, "RFCOMM") == 0) {
        // probably SPP, possibly other RFCOMM-based connections
        if (channel == 1) {
            iwrap_connection_map[pairing_index] -> link_spp = link_id;
        //} else {
            //printf("RFCOMM link on channel %d, profile unknown\n", channel);
        }
    }
}

uint8_t remove_mapped_connection(uint8_t link_id) {
    uint8_t i;
    for (i = 0; i < iwrap_pairings; i++) {
        if (iwrap_connection_map[i] -> link_a2dp1 == link_id) { iwrap_connection_map[i] -> link_a2dp1 = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_a2dp2 == link_id) { iwrap_connection_map[i] -> link_a2dp2 = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_avrcp == link_id) { iwrap_connection_map[i] -> link_avrcp = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_hfp == link_id) { iwrap_connection_map[i] -> link_hfp = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_hfpag == link_id) { iwrap_connection_map[i] -> link_hfpag = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_hid_control == link_id) { iwrap_connection_map[i] -> link_hid_control = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_hid_interrupt == link_id) { iwrap_connection_map[i] -> link_hid_interrupt = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_hsp == link_id) { iwrap_connection_map[i] -> link_hsp = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_hspag == link_id) { iwrap_connection_map[i] -> link_hspag = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_iap == link_id) { iwrap_connection_map[i] -> link_iap = 0xFF; break; }
        if (iwrap_connection_map[i] -> link_spp == link_id) { iwrap_connection_map[i] -> link_spp = 0xFF; break; }
    }
    
    // check to make sure we found the link ID in the map
    if (i < iwrap_pairings) {
        // updated connected device count and overall active link count for this device
        if (iwrap_connection_map[i] -> active_links) {
            iwrap_connection_map[i] -> active_links--;
            if (!iwrap_connection_map[i] -> active_links) iwrap_connected_devices--;
        }
        return i;
    }
    
    // not found, return 0xFF
    return 0xFF;
}

/* ============================================================================
 * PLATFORM-SPECIFIC HELPER FUNCTIONS
 * ========================================================================= */


    int serial_out(const char *str) {
        // debug output to host goes through hardware serial
        return Serial.print(str);
    }
    int serial_out(const __FlashStringHelper *str) {
        // debug output to host goes through hardware serial
        return Serial.print(str);
    }
    int iwrap_out(int len, unsigned char *data) {
        // iWRAP output to module goes through software serial
        return Serial1.write(data, len);
    }

    
void print_connection_map() {
    char s[100];
    serial_out(F("==============================================================================\n"));
    sprintf(s, "Connection map (%d pairings, %d connected devices, %d total connections)\n", iwrap_pairings, iwrap_connected_devices, iwrap_active_connections);
    serial_out(s);
    serial_out(F("--------+-------+-------+-----+-------+-------+-----+-------+-----+-----+-----\n"));
    serial_out(F("Device\t|  A2DP | AVRCP | HFP | HFPAG |  HID  | HSP | HSPAG | IAP | SCO | SPP\n"));
    serial_out(F("--------+-------+-------+-----+-------+-------+-----+-------+-----+-----+-----\n"));
    uint8_t i;
    for (i = 0; i < iwrap_pairings; i++) {
        sprintf(s, "#%d (%d)\t| %2d/%2d |  %2d   |  %2d |   %2d  | %2d/%2d |  %2d |   %2d  |  %2d |  %2d |  %2d\n", i, iwrap_connection_map[i] -> active_links,
            (int8_t)iwrap_connection_map[i] -> link_a2dp1,
            (int8_t)iwrap_connection_map[i] -> link_a2dp2,
            (int8_t)iwrap_connection_map[i] -> link_avrcp,
            (int8_t)iwrap_connection_map[i] -> link_hfp,
            (int8_t)iwrap_connection_map[i] -> link_hfpag,
            (int8_t)iwrap_connection_map[i] -> link_hid_control,
            (int8_t)iwrap_connection_map[i] -> link_hid_interrupt,
            (int8_t)iwrap_connection_map[i] -> link_hsp,
            (int8_t)iwrap_connection_map[i] -> link_hspag,
            (int8_t)iwrap_connection_map[i] -> link_iap,
            (int8_t)iwrap_connection_map[i] -> link_sco,
            (int8_t)iwrap_connection_map[i] -> link_spp
            );
        serial_out(s);
    }
    serial_out(F("--------+-------+-------+-----+-------+-------+-----+-------+-----+-----+-----\n"));
}

void print_demo_menu() {
    serial_out(F("iWRAP generic demo menu\n"));
    serial_out(F("============================================================\n"));
    serial_out(F("0: Reset module (pulses iWRAP module RESET pin high for 5ms)\n"));
    serial_out(F("1: Toggle round-robin autocall algorithm on or off\n"));
    serial_out(F("2: Make module discoverable and connectable (SET BT PAGE 3)\n"));
    serial_out(F("3: Make module undiscoverable, but connectable (SET BT PAGE 2)\n"));
    serial_out(F("4: Make module undiscoverable and unconnectable (SET BT PAGE 0)\n"));
}

void process_demo_choice(char b) {
    // check for RESET command ("0")
    if (b == '0') {
        serial_out("=> (0) Performing hardware reset on iWRAP module\n\n");
        digitalWrite(MODULE_RESET_PIN, HIGH);
        delay(5);
        digitalWrite(MODULE_RESET_PIN, LOW);
        iwrap_pending_commands = 0; // normally handled by the parser, but this is a hard reset
    } else if (b == '1') {
        if (iwrap_autocall_target) {
            serial_out("=> (1) Disabling round-robin autocall algorithm\n\n");
            iwrap_autocall_target = 0;
        } else {
            serial_out("=> (1) Enabling round-robin autocall algorithm\n\n");
            iwrap_autocall_target = 1;
        }
    } else if (b == '2') {
        serial_out("=> (2) Setting page mode to 3 (discoverable and connectable)\n\n");
        iwrap_send_command("SET BT PAGEMODE 3", iwrap_mode);
    } else if (b == '3') {
        serial_out("=> (3) Setting page mode to 2 (undiscoverable, but connectable)\n\n");
        iwrap_send_command("SET BT PAGEMODE 2", iwrap_mode);
    } else if (b == '4') {
        serial_out("=> (4) Setting page mode to 0 (undiscoverable and unconnectable)\n\n");
        iwrap_send_command("SET BT PAGEMODE 0", iwrap_mode);
    } else if (b =='5') {
        iwrap_send_command("AVRCP PDU 31 1 2", IWRAP_MODE_MUX);
    } else if (b =='6') {
        iwrap_send_command("AVRCP PDU 20 1 1", IWRAP_MODE_MUX);
    } else if (b =='7') {
        iwrap_send_command("AVRCP PDU 20 1 2", IWRAP_MODE_MUX);
    } else if (b =='8') {
        iwrap_send_command("AVRCP PDU 31 1", IWRAP_MODE_MUX);
    } else if (b =='9') {
        iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);
    }
}
