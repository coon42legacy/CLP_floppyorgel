#include "Energia.h"

#define GAMEPAD_DATA PM_0
#define GAMEPAD_LATCH PM_1
#define GAMEPAD_CLOCK PM_2

typedef struct {
  byte
  East : 1,
  West : 1,
  South : 1,
  North : 1,
  Start : 1,
  Select : 1,
  B : 1,
  A : 1;
} NesGamePadButtonStates_t;

union NesGamePadStates_t {
  NesGamePadButtonStates_t states;
  byte code;
};

void setupGamePad();
NesGamePadStates_t getGamepadState();
static void gamepadLatchShiftRegister_();
