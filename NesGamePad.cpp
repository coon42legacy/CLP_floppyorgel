#include "NesGamepad.h"

void setupGamePad() {
  pinMode(GAMEPAD_DATA, INPUT);
  pinMode(GAMEPAD_LATCH, OUTPUT);
  pinMode(GAMEPAD_CLOCK, OUTPUT);
  
  digitalWrite(GAMEPAD_LATCH, LOW);
  digitalWrite(GAMEPAD_CLOCK, HIGH);
}

void gamepadLatchShiftRegister_() {
  digitalWrite(GAMEPAD_LATCH, HIGH);
  delayMicroseconds(12);
  digitalWrite(GAMEPAD_LATCH, LOW);
}

NesGamePadStates_t getGamepadState() {
  gamepadLatchShiftRegister_();
  NesGamePadStates_t states;
  states.code = digitalRead(GAMEPAD_DATA) ^ 1;
  
  for(int i = 0; i < 8; i++) {
    states.code <<= 1;
    digitalWrite(GAMEPAD_CLOCK, LOW);
    delayMicroseconds(12);
    states.code |= (digitalRead(GAMEPAD_DATA) ^ 1);
    digitalWrite(GAMEPAD_CLOCK, HIGH);
    delayMicroseconds(12);
  }
  
  return states;
}
