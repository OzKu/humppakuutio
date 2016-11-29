#include "State.h"

State *State::instance = NULL;

State *State::getInstance() {
  if (!instance) {
    instance = new State();
  }
  return instance;
}
