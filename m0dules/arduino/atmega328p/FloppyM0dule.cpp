#include "FloppyM0dule.h"
#include "FloppyM0dule_conf.h"

static void driveSelect(boolean enable) {
  digitalWrite(PIN_DRIVE_SELECT, enable ? LOW : HIGH);
}

static void setDirection(int headDirection) {
  currentDirection = headDirection;
  digitalWrite(PIN_DIRECTION, headDirection == FORWARD ? LOW : HIGH);  
}

static void toggleStep() {  
  // Switch directions if end has been reached
  if (currentPosition >= MAX_STEP_POSITION)
    setDirection(BACKWARD);
  else if (currentPosition <= 0)
    setDirection(FORWARD);
    
  currentPosition = currentDirection == FORWARD ? currentPosition + 1 : currentPosition - 1;
  
  // Pulse the step pin
  digitalWrite(PIN_STEP, currentStepPinState ? LOW : HIGH);  
  currentStepPinState = ~currentStepPinState;
}

// Stops playing of current note and set read/write head back to track 0.
void resetDrive() {
  currentPeriod = 0; // Stop playing note
  
  // Set read / write head to track 0
  driveSelect(true);
  for (uint8_t i = 0; i < 80; i++) { // For max drive's position
    setDirection(BACKWARD);
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(1500);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(1500);
  }
  driveSelect(false);

  currentPosition = 0; // We're reset.
  setDirection(FORWARD); // Ready to go forward.
}

void tick() {
  // If there is a period set for control pin 2, count the number of
  // ticks that pass, and toggle the pin if the current period is reached.
  if (currentPeriod > 0) {
      currentTick++;
      
    if (currentTick >= currentPeriod) {
      toggleStep();
      currentTick = 0;
    }
  }
}

void playMidiNote(uint16_t midi_note) {
  currentPeriod = microPeriods[midi_note] / (2 * MOPPYEX_RESOLUTION);
  driveSelect(currentPeriod ? true : false);
}
