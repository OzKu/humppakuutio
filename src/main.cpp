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
#include <Arduino.h>
#include <SPI.h>

#include "iWRAP.h"
#include "SimpleTimer.h"
#include "ScrollingText.h"
#include "Display.h"
#include "Colors.h"
#include "State.h"

ScrollingText *titleText;
ScrollingText *artistText;
SimpleTimer timer;
Display display;

// SoftwareSerial Serial1(10, 2);
unsigned long textChanged = 0;

// IWRAP
#define IWRAP_STATE_IDLE            0
#define IWRAP_STATE_UNKNOWN         1
#define IWRAP_STATE_PENDING_AT      2
#define IWRAP_STATE_PENDING_SET     3
#define IWRAP_STATE_PENDING_LIST    4
#define IWRAP_STATE_PENDING_CALL    5
#define IWRAP_STATE_COMM_FAILED     255

#define IWRAP_MAX_PAIRINGS          16

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

void requestSongInfo(){
  iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);  // request track change
  iwrap_send_command("AVRCP PDU 20 1 1", IWRAP_MODE_MUX);  // request title
  iwrap_send_command("AVRCP PDU 20 1 2", IWRAP_MODE_MUX);  // request artist
  iwrap_send_command("AVRCP PDU 31 1 1", IWRAP_MODE_MUX);  // request play pause
}

void my_iwrap_evt_a2dp_streaming_start(uint8_t link_id) {
  Serial.println("!! streaming start");
  // paused=false;
  // drawPlayPause(false);
  // requestSongInfo();
  timer.setTimeout(100, requestSongInfo);
  #ifdef LCD
  if (display.getScreen() != PLAY) display.drawPlayMenu();
  #endif
}

void my_iwrap_evt_a2dp_streaming_stop(uint8_t link_id) {
  Serial.println("!! streaming stop");
  // paused=true;
  // drawPlayPause(false);
}

void my_iwrap_evt_no_carrier(uint8_t link_id,uint16_t error_code, const char *message) {
  Serial.println("Disconnected!");
  if(millis()-State::getInstance()->lastIntro > 500) display.drawIntro();
}

void my_iwrap_evt_track_changed() {
  Serial.println("!! track changed");
  requestSongInfo();
}

void my_iwrap_evt_paused() {
  Serial.println("!! PAUSE!!!!!!");
  State::getInstance()->paused = true;
  display.drawPlayPause(false);
  iwrap_send_command("AVRCP PDU 31 1 1", IWRAP_MODE_MUX); // request play pause
}

void my_iwrap_evt_playing() {
  Serial.println("!! PLAY!!!!!!");
  State::getInstance()->paused = false;
  display.drawPlayPause(false);
  iwrap_send_command("AVRCP PDU 31 1 1", IWRAP_MODE_MUX); // request play pause
}

void my_iwrap_evt_title_received(const char *rawTitle) {
  String title = String(rawTitle);
  title = title.substring(48, title.length() - 3);

  const char aa[] = { 0x1A, 0x0 };
  const char bb[] = { 0x1B, 0x0 };
  const char cc[] = { 0x1C, 0x0 };
  const char AA[] = { 0x1D, 0x0 };
  const char BB[] = { 0x1E, 0x0 };
  const char CC[] = { 0x1F, 0x0 };

  title.replace("ä", aa);
  title.replace("å", bb);
  title.replace("ö", cc);
  title.replace("Ä", AA);
  title.replace("Å", BB);
  title.replace("Ö", CC);
  title.replace("’", "'");

  titleText->setText(title);

  Serial.println("title: " + title);
  textChanged = millis();
}

void my_iwrap_evt_artist_received(const char *rawArtist) {
  String artist = String(rawArtist);
  artist = artist.substring(49, artist.length() - 3);
  Serial.println("artisti makssaaaaa: '" + artist + "'");
  textChanged = millis();

  const char aa[] = { 0x1A, 0x0 };
  const char bb[] = { 0x1B, 0x0 };
  const char cc[] = { 0x1C, 0x0 };
  const char AA[] = { 0x1D, 0x0 };
  const char BB[] = { 0x1E, 0x0 };
  const char CC[] = { 0x1F, 0x0 };

  artist.replace("ä", aa);
  artist.replace("å", bb);
  artist.replace("ö", cc);
  artist.replace("Ä", AA);
  artist.replace("Å", BB);
  artist.replace("Ö", CC);
  artist.replace("’", "'");

  artistText->setText(artist);
}

void onPreviousClick() {
  Serial.println("Previous");
  display.drawPreviousButton(true);
  iwrap_send_command("AVRCP BACKWARD", IWRAP_MODE_MUX);
}

void onPlayPauseClick() {
  if (State::getInstance()->paused) {
    Serial.println("Play");
    iwrap_send_command("AVRCP PLAY", IWRAP_MODE_MUX);
  } else {
    Serial.println("Pause");
    iwrap_send_command("AVRCP PAUSE", IWRAP_MODE_MUX);
  }
  display.drawPlayPause(false);
  State::getInstance()->paused = !State::getInstance()->paused;
}

void onNextClick() {
  Serial.println("Next");
  iwrap_send_command("AVRCP FORWARD", IWRAP_MODE_MUX);
  display.drawNextButton(true);
}

void onMenuOptionClick(unsigned int optionIndex) {
  switch (optionIndex) {
    case 0:
      iwrap_send_command("SET BT PAGEMODE 3", iwrap_mode);
      State::getInstance()->pageMode = 0;
      break;
    case 1:
      iwrap_send_command("SET BT PAGEMODE 2", iwrap_mode);
      State::getInstance()->pageMode = 1;
      break;
    case 2:
      iwrap_send_command("SET BT PAGEMODE 0", iwrap_mode);
      State::getInstance()->pageMode = 2;
      break;
    case 3:
      if (State::getInstance()->iwrap_autocall_target) {
          Serial.println("=> (1) Disabling round-robin autocall algorithm\n\n");
          State::getInstance()->iwrap_autocall_target = 0;
      } else {
          Serial.println("=> (1) Enabling round-robin autocall algorithm\n\n");
          State::getInstance()->iwrap_autocall_target = 1;
      }
      break;
  }
}

void setup() {
  Serial.begin(HOST_BAUD);
  Serial1.begin(IWRAP_BAUD);

  digitalWrite(MODULE_RESET_PIN, LOW);
  pinMode(MODULE_RESET_PIN, OUTPUT);

  titleText = new ScrollingText(display.getLcd());
  titleText->setText("");
  titleText->setPosition(20, 20);
  titleText->setColor(COLOR_WHITE);
  titleText->setBackgroundColor(COLOR_BG);

  artistText = new ScrollingText(display.getLcd());
  artistText->setText("");
  artistText->setPosition(20, 50);
  artistText->setColor(COLOR_WHITE);
  artistText->setBackgroundColor(COLOR_BG);

  timer.setInterval(5000, requestSongInfo);

  // assign transport/debug output
  iwrap_output = iwrap_out;
  #ifdef IWRAP_DEBUG
      iwrap_debug = serial_out;
  #endif // IWRAP_DEBUG

  serial_out(F("iWRAP host library generic demo started\n"));

  #ifdef LCD
  display.setupLcd();
  display.drawIntro();
  display.onPreviousClick = onPreviousClick;
  display.onPlayPauseClick = onPlayPauseClick;
  display.onNextClick = onNextClick;
  #endif

  iwrap_evt_a2dp_streaming_start = my_iwrap_evt_a2dp_streaming_start;
  iwrap_evt_a2dp_streaming_stop = my_iwrap_evt_a2dp_streaming_stop;
  iwrap_evt_track_changed = my_iwrap_evt_track_changed;
  iwrap_evt_paused = my_iwrap_evt_paused;
  iwrap_evt_playing = my_iwrap_evt_playing;
  iwrap_evt_title_received = my_iwrap_evt_title_received;
  iwrap_evt_artist_received = my_iwrap_evt_artist_received;
  iwrap_evt_no_carrier = my_iwrap_evt_no_carrier;
}

void loop() {
  #ifdef LCD
  display.update();

  if (display.getScreen() == PLAY) {
    titleText->update();
    artistText->update();
  }
  #endif

  timer.run();

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
            if (iwrap_pairings && State::getInstance()->iwrap_autocall_target > iwrap_connected_devices && !iwrap_pending_calls
                               && (!iwrap_autocall_last_time || (millis() - iwrap_autocall_last_time) >= iwrap_autocall_delay_ms)) {
                char cmd[] = "CALL AA:BB:CC:DD:EE:FF 19 A2DP";      // A2DP
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 17 AVRCP";     // AVRCP
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 111F HFP";     // HFP
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 111E HFP-AG";  // HFP-AG
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 11 HID";       // HID
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 1112 HSP";     // HSP
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF 1108 HSP-AG";  // HSP-AG
                //char cmd[] = "CALL AA:BB:CC:DD:EE:FF * IAP";        // IAP
                // char cmd[] = "CALL AA:BB:CC:DD:EE:FF 1101 RFCOMM";  // SPP
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
        if (State::getInstance()->iwrap_autocall_target) {
            serial_out("=> (1) Disabling round-robin autocall algorithm\n\n");
            State::getInstance()->iwrap_autocall_target = 0;
        } else {
            serial_out("=> (1) Enabling round-robin autocall algorithm\n\n");
            State::getInstance()->iwrap_autocall_target = 1;
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
        //drawIntro();
        display.drawSetupMenu(4);
    } else if (b =='6') {
        iwrap_send_command("AVRCP PDU 20 1 1", IWRAP_MODE_MUX);
    } else if (b =='7') {
        iwrap_send_command("AVRCP PDU 20 1 2", IWRAP_MODE_MUX);
    } else if (b =='8') {
        iwrap_send_command("AVRCP PDU 31 1", IWRAP_MODE_MUX);
    } else if (b =='9') {
        iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);
    } else if (b == 'a') {
      iwrap_send_command("AVRCP PDU 31 1 1", IWRAP_MODE_MUX);
    }
}
