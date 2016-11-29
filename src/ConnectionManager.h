#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <Arduino.h>
#include "iWRAP.h"

#define IWRAP_MAX_PAIRINGS (16)

// IWRAP
#define IWRAP_STATE_IDLE            (0)
#define IWRAP_STATE_UNKNOWN         (1)
#define IWRAP_STATE_PENDING_AT      (2)
#define IWRAP_STATE_PENDING_SET     (3)
#define IWRAP_STATE_PENDING_LIST    (4)
#define IWRAP_STATE_PENDING_CALL    (5)
#define IWRAP_STATE_COMM_FAILED     (255)

class ConnectionManager {
public:
  void update();
  uint8_t sendCommand(const char *cmd);
  void play();
  void pause();
  void backward();
  void forward();
  void setPageMode(unsigned int pageMode);
  void enableAutoCall(bool enable);
private:
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
  iwrap_connection_t *connectionMap[IWRAP_MAX_PAIRINGS];
  uint8_t mode = IWRAP_MODE_MUX;
  uint8_t state = IWRAP_STATE_UNKNOWN;
  uint8_t initialized = 0;
  uint32_t time_ref = 0;
  uint8_t pairings = 0;
  uint8_t pending_calls = 0;
  uint8_t pending_call_link_id = 0xFF;
  uint8_t connected_devices = 0;
  uint8_t active_connections = 0;
  uint16_t autocall_delay_ms = 10000;
  uint32_t autocall_last_time = 0;
  uint8_t autocall_index = 0;
  uint8_t findPairingFromMac(const iwrap_address_t *mac);
  uint8_t findPairingFromLinkId(uint8_t link_id);
  void addMappedConnection(uint8_t link_id, const iwrap_address_t *addr, const char *mode, uint16_t channel);
  uint8_t removeMappedConnection(uint8_t link_id);
  void printConnectionMap();
  void printDemoMenu();
};

#endif
