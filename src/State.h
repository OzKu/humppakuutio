#pragma once

#include <Arduino.h>

/**
 * Global application state, singleton
 */

// TODO: Code should be refactored so that as few variables are possible are needed here
class State {
public:
  static State *getInstance();
  bool paused = false;
  uint8_t iwrap_autocall_target = 0;
  unsigned long lastIntro = 0;
  unsigned int pageMode = -1;
private:
  static State *instance;
};
