#include "ConnectionManager.h"

#include "State.h"

void ConnectionManager::play() {
  sendCommand("AVRCP PLAY");
}

void ConnectionManager::pause() {
  sendCommand("AVRCP PAUSE");
}

void ConnectionManager::backward() {
  sendCommand("AVRCP BACKWARD");
}

void ConnectionManager::forward() {
  sendCommand("AVRCP FORWARD");
}

void ConnectionManager::setPageMode(unsigned int pageMode) {
  char command[30];
  sprintf(command, "SET BT PAGEMODE %d", pageMode);
  sendCommand(command);
}

void ConnectionManager::enableAutoCall(bool enable) {
  
}

void ConnectionManager::update() {
  // manage iWRAP state machine
  if (!iwrap_pending_commands) {
    // no pending commands, some state transition occurring
    if (state) {
      // not idle, in the middle of some process
      if (state == IWRAP_STATE_UNKNOWN) {
        // reset all detailed state trackers
        initialized = 0;
        pairings = 0;
        pending_calls = 0;
        pending_call_link_id = 0xFF;
        connected_devices = 0;
        active_connections = 0;

        // send command to test module connectivity
        Serial.print(F("Testing iWRAP communication...\n"));
        iwrap_send_command("AT", mode);
        state = IWRAP_STATE_PENDING_AT;

        // initialize time reference for connectivity test timeout
        time_ref = millis();
      } else if (state == IWRAP_STATE_PENDING_AT) {
        // send command to dump all module settings and pairings
        Serial.print(F("Getting iWRAP settings...\n"));
        iwrap_send_command("SET", mode);
        state = IWRAP_STATE_PENDING_SET;
      } else if (state == IWRAP_STATE_PENDING_SET) {
        // send command to show all current connections
        Serial.print(F("Getting active connection list...\n"));
        iwrap_send_command("LIST", mode);
        state = IWRAP_STATE_PENDING_LIST;
      } else if (state == IWRAP_STATE_PENDING_LIST) {
        // all done!
        if (!initialized) {
            initialized = 1;
            Serial.print(F("iWRAP initialization complete\n"));
        }
        printConnectionMap();
        printDemoMenu();
        iwrap_send_command("AVRCP PDU 31 2", IWRAP_MODE_MUX);
        state = IWRAP_STATE_IDLE;
      } else if (state == IWRAP_STATE_PENDING_CALL && !pending_calls) {
        // all done!
        Serial.print(F("Pending call processed\n"));
        state = IWRAP_STATE_IDLE;
      }
    } else if (initialized) {
      // idle
      if (pairings && State::getInstance()->iwrap_autocall_target > connected_devices && !pending_calls
                         && (!autocall_last_time || (millis() - autocall_last_time) >= autocall_delay_ms)) {
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
        for (; connectionMap[autocall_index]->active_links; autocall_index++);

        // write MAC string into call command buffer and send it
        iwrap_bintohexstr((uint8_t *)(connectionMap[autocall_index]->mac.address), 6, &cptr, ':', 0);
        char s[21];
        sprintf(s, "Calling device #%d\r\n", autocall_index);
        Serial.print(s);
        iwrap_send_command(cmd, mode);
        autocall_last_time = millis();
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
  if (!initialized && state == IWRAP_STATE_PENDING_AT) {
      if (millis() - time_ref > 5000) {
          Serial.print(F("ERROR: Could not communicate with iWRAP module\n"));
          state = IWRAP_STATE_COMM_FAILED;
          iwrap_pending_commands = 0; // normally handled by the parser, but comms failed
      }
  }
}

uint8_t ConnectionManager::sendCommand(const char *cmd) {
  return iwrap_send_command(cmd, mode);
}

uint8_t ConnectionManager::findPairingFromMac(const iwrap_address_t *mac) {
    uint8_t i;
    for (i = 0; i < pairings; i++) {
        if (memcmp(&(connectionMap[i]->mac), mac, sizeof(iwrap_address_t)) == 0) return i;
    }
    return i >= pairings ? 0xFF : i;
}

uint8_t ConnectionManager::findPairingFromLinkId(uint8_t link_id) {
    // NOTE: This implementation walks through the iwrap_connection_t struct memory
    // starting at the first link ID (A2DP #1) and going all the way to the end (if
    // necessary). This means it is contingent on the structure keeping this memory
    // arrangement, where the last set of whatever is contained there is always the
    // set of link IDs.

    uint8_t i, j, *idptr;
    for (i = 0; i < pairings; i++) {
        idptr = &(connectionMap[i]->link_a2dp1);
        for (j = 6; j < sizeof(iwrap_connection_t); j++, idptr++) if (idptr[0] == link_id) return i;
    }
    return 0xFF;
}

void ConnectionManager::addMappedConnection(uint8_t link_id, const iwrap_address_t *addr, const char *mode, uint16_t channel) {
    uint8_t pairing_index = findPairingFromMac(addr);

    // make sure we found a match (we SHOULD always match something, if we properly parsed the pairing data first)
    if (pairing_index == 0xFF) return; // uh oh

    // updated connected device count and overall active link count for this device
    if (!connectionMap[pairing_index]->active_links) connected_devices++;
    connectionMap[pairing_index]->active_links++;

    // add link ID to connection map
    if (strcmp(mode, "A2DP") == 0) {
        if (connectionMap[pairing_index]->link_a2dp1 == 0xFF) {
            connectionMap[pairing_index]->link_a2dp1 = link_id;
        } else {
            connectionMap[pairing_index]->link_a2dp2 = link_id;
        }
    } else if (strcmp(mode, "AVRCP") == 0) {
        connectionMap[pairing_index]->link_avrcp = link_id;
    } else if (strcmp(mode, "HFP") == 0) {
        connectionMap[pairing_index]->link_hfp = link_id;
    } else if (strcmp(mode, "HFPAG") == 0) {
        connectionMap[pairing_index]->link_hfpag = link_id;
    } else if (strcmp(mode, "HID") == 0) {
        if (channel == 0x11) {
            connectionMap[pairing_index]->link_hid_control = link_id;
        } else {
            connectionMap[pairing_index]->link_hid_interrupt = link_id;
        }
    } else if (strcmp(mode, "HSP") == 0) {
        connectionMap[pairing_index]->link_hsp = link_id;
    } else if (strcmp(mode, "HSPAG") == 0) {
        connectionMap[pairing_index]->link_hspag = link_id;
    } else if (strcmp(mode, "IAP") == 0) {
        connectionMap[pairing_index]->link_iap = link_id;
    } else if (strcmp(mode, "RFCOMM") == 0) {
        // probably SPP, possibly other RFCOMM-based connections
        if (channel == 1) {
            connectionMap[pairing_index]->link_spp = link_id;
        //} else {
            //printf("RFCOMM link on channel %d, profile unknown\n", channel);
        }
    }
}

uint8_t ConnectionManager::removeMappedConnection(uint8_t link_id) {
  uint8_t i;
  for (i = 0; i < pairings; i++) {
    if (connectionMap[i]->link_a2dp1 == link_id) { connectionMap[i]->link_a2dp1 = 0xFF; break; }
    if (connectionMap[i]->link_a2dp2 == link_id) { connectionMap[i]->link_a2dp2 = 0xFF; break; }
    if (connectionMap[i]->link_avrcp == link_id) { connectionMap[i]->link_avrcp = 0xFF; break; }
    if (connectionMap[i]->link_hfp == link_id) { connectionMap[i]->link_hfp = 0xFF; break; }
    if (connectionMap[i]->link_hfpag == link_id) { connectionMap[i]->link_hfpag = 0xFF; break; }
    if (connectionMap[i]->link_hid_control == link_id) { connectionMap[i]->link_hid_control = 0xFF; break; }
    if (connectionMap[i]->link_hid_interrupt == link_id) { connectionMap[i]->link_hid_interrupt = 0xFF; break; }
    if (connectionMap[i]->link_hsp == link_id) { connectionMap[i]->link_hsp = 0xFF; break; }
    if (connectionMap[i]->link_hspag == link_id) { connectionMap[i]->link_hspag = 0xFF; break; }
    if (connectionMap[i]->link_iap == link_id) { connectionMap[i]->link_iap = 0xFF; break; }
    if (connectionMap[i]->link_spp == link_id) { connectionMap[i]->link_spp = 0xFF; break; }
  }

  // check to make sure we found the link ID in the map
  if (i < pairings) {
    // updated connected device count and overall active link count for this device
    if (connectionMap[i]->active_links) {
      connectionMap[i]->active_links--;
      if (!connectionMap[i]->active_links) connected_devices--;
    }
    return i;
  }

  // not found, return 0xFF
  return 0xFF;
}

void ConnectionManager::printConnectionMap() {
  char s[100];
  Serial.print(F("==============================================================================\n"));
  sprintf(s, "Connection map (%d pairings, %d connected devices, %d total connections)\n", pairings, connected_devices, active_connections);
  Serial.print(s);
  Serial.print(F("--------+-------+-------+-----+-------+-------+-----+-------+-----+-----+-----\n"));
  Serial.print(F("Device\t|  A2DP | AVRCP | HFP | HFPAG |  HID  | HSP | HSPAG | IAP | SCO | SPP\n"));
  Serial.print(F("--------+-------+-------+-----+-------+-------+-----+-------+-----+-----+-----\n"));
  uint8_t i;
  for (i = 0; i < pairings; i++) {
    sprintf(s, "#%d (%d)\t| %2d/%2d |  %2d   |  %2d |   %2d  | %2d/%2d |  %2d |   %2d  |  %2d |  %2d |  %2d\n",
      i, connectionMap[i]->active_links,
      (int8_t)connectionMap[i]->link_a2dp1,
      (int8_t)connectionMap[i]->link_a2dp2,
      (int8_t)connectionMap[i]->link_avrcp,
      (int8_t)connectionMap[i]->link_hfp,
      (int8_t)connectionMap[i]->link_hfpag,
      (int8_t)connectionMap[i]->link_hid_control,
      (int8_t)connectionMap[i]->link_hid_interrupt,
      (int8_t)connectionMap[i]->link_hsp,
      (int8_t)connectionMap[i]->link_hspag,
      (int8_t)connectionMap[i]->link_iap,
      (int8_t)connectionMap[i]->link_sco,
      (int8_t)connectionMap[i]->link_spp
    );
    Serial.print(s);
  }
  Serial.print(F("--------+-------+-------+-----+-------+-------+-----+-------+-----+-----+-----\n"));
}

void ConnectionManager::printDemoMenu() {
  Serial.print(F("iWRAP generic demo menu\n"));
  Serial.print(F("============================================================\n"));
  Serial.print(F("0: Reset module (pulses iWRAP module RESET pin high for 5ms)\n"));
  Serial.print(F("1: Toggle round-robin autocall algorithm on or off\n"));
  Serial.print(F("2: Make module discoverable and connectable (SET BT PAGE 3)\n"));
  Serial.print(F("3: Make module undiscoverable, but connectable (SET BT PAGE 2)\n"));
  Serial.print(F("4: Make module undiscoverable and unconnectable (SET BT PAGE 0)\n"));
}
