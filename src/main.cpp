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

#include <Arduino.h>
#include <GraphicsLib.h>
#include <MI0283QT9.h>
#include <SPI.h>
#include <SimpleTimer.h>

#include "Colors.h"
#include "ConnectionManager.h"
#include "Display.h"
#include "iWRAP.h"
#include "ScrollingText.h"
#include "State.h"

SimpleTimer timer;
Display display;
ConnectionManager connectionManager;
ScrollingText *titleText;
ScrollingText *artistText;

// general helper functions
void process_demo_choice(char b);

// platform-specific helper functions
int serial_out(const char *str);
int serial_out(const __FlashStringHelper *str);
int iwrap_out(int len, unsigned char *data);

void requestSongInfo() {
  iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);  // request track change
  iwrap_send_command("AVRCP PDU 20 1 1", IWRAP_MODE_MUX);  // request title
  iwrap_send_command("AVRCP PDU 20 1 2", IWRAP_MODE_MUX);  // request artist
  iwrap_send_command("AVRCP PDU 31 1 1", IWRAP_MODE_MUX);  // request play pause
}

void onStreamingStart(uint8_t link_id) {
  Serial.println("[EVENT] STREAMING START");
  // paused=false;
  // drawPlayPause(false);
  // requestSongInfo();
  timer.setTimeout(100, requestSongInfo);
  #ifdef LCD
  if (display.getScreen() != PLAY) {
    display.drawPlayMenu();
  }
  #endif
}

void onStreamingStop(uint8_t link_id) {
  Serial.println("[EVENT] STREAMING STOP");
  // paused=true;
  // drawPlayPause(false);
}

void onDisconnect(uint8_t link_id,uint16_t error_code, const char *message) {
  Serial.println("[EVENT] DISCONNECT");
  if(millis()-State::getInstance()->lastIntro > 500) display.drawIntro();
}

void onTrackChange() {
  Serial.println("[EVENT] TRACK CHANGE");
  requestSongInfo();
}

void onPause() {
  Serial.println("[EVENT] PAUSE");
  State::getInstance()->paused = true;
  display.drawPlayPause(false);
  iwrap_send_command("AVRCP PDU 31 1 1", IWRAP_MODE_MUX); // request play pause
}

void onPlay() {
  Serial.println("[EVENT] PLAY");
  State::getInstance()->paused = false;
  display.drawPlayPause(false);
  iwrap_send_command("AVRCP PDU 31 1 1", IWRAP_MODE_MUX); // request play pause
}

void onTitleReceived(const char *rawTitle) {
  String title = String(rawTitle);
  title = title.substring(48, title.length() - 3);

  // TODO: Move to a function
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
  Serial.println("TITLE RECEIVED: '" + title + "'");
}

void onArtistReceived(const char *rawArtist) {
  String artist = String(rawArtist);
  artist = artist.substring(49, artist.length() - 3);

  // TODO: Move to a function
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
  Serial.println("ARTIST RECEIVED: '" + artist + "'");
}

void onPreviousClick() {
  Serial.println("Previous");
  display.drawPreviousButton(true);
  connectionManager.backward();
}

void onPlayPauseClick() {
  if (State::getInstance()->paused) {
    Serial.println("Play");
    connectionManager.play();
  } else {
    Serial.println("Pause");
    connectionManager.pause();
  }
  display.drawPlayPause(false);
  State::getInstance()->paused = !State::getInstance()->paused;
}

void onNextClick() {
  Serial.println("Next");
  connectionManager.forward();
  display.drawNextButton(true);
}

void onMenuOptionClick(unsigned int optionIndex) {
  switch (optionIndex) {
    case 0:
      connectionManager.setPageMode(3);
      State::getInstance()->pageMode = 0; // TODO: Why does this not match the actual page mode?
      break;
    case 1:
      connectionManager.setPageMode(2);
      State::getInstance()->pageMode = 1;
      break;
    case 2:
      connectionManager.setPageMode(0);
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

  // Setup display
  #ifdef LCD
  display.setupLcd();
  display.drawIntro();

  // Bind display callbacks
  display.onPreviousClick = onPreviousClick;
  display.onPlayPauseClick = onPlayPauseClick;
  display.onNextClick = onNextClick;
  display.onMenuOptionClick = onMenuOptionClick;

  // Create title and artist texts
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
  #endif

  // Assign transport/debug output
  iwrap_output = iwrap_out;
  #ifdef IWRAP_DEBUG
    iwrap_debug = serial_out;
  #endif

  // Bind iWRAP callbacks
  iwrap_evt_a2dp_streaming_start = onStreamingStart;
  iwrap_evt_a2dp_streaming_stop = onStreamingStop;
  iwrap_evt_track_changed = onTrackChange;
  iwrap_evt_paused = onPause;
  iwrap_evt_playing = onPlay;
  iwrap_evt_title_received = onTitleReceived;
  iwrap_evt_artist_received = onArtistReceived;
  iwrap_evt_no_carrier = onDisconnect;

  // Poll artist and title
  timer.setInterval(5000, requestSongInfo);
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
  connectionManager.update();

  // check for incoming host data
  uint16_t result;
  if ((result = Serial.read()) < 256)
  {
    process_demo_choice(result & 0xFF);
  }
}

int serial_out(const char *str) {
  return Serial.print(str);
}

int serial_out(const __FlashStringHelper *str) {
  return Serial.print(str);
}

int iwrap_out(int len, unsigned char *data) {
  return Serial1.write(data, len);
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
    connectionManager.sendCommand("SET BT PAGEMODE 3");
  } else if (b == '3') {
    serial_out("=> (3) Setting page mode to 2 (undiscoverable, but connectable)\n\n");
    connectionManager.sendCommand("SET BT PAGEMODE 2");
  } else if (b == '4') {
    serial_out("=> (4) Setting page mode to 0 (undiscoverable and unconnectable)\n\n");
    connectionManager.sendCommand("SET BT PAGEMODE 0");
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
