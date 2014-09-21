#include "FloppyM0dule.h"
#include "FloppyM0dule_conf.h"

void initFloppyM0dule() {
  pinMode(PIN_DRIVE_SELECT, OUTPUT); 
  pinMode(PIN_DIRECTION,    OUTPUT); 
  pinMode(PIN_STEP,         OUTPUT);  
  digitalWrite(PIN_DRIVE_SELECT, LOW);
  digitalWrite(PIN_DIRECTION,    LOW);
  digitalWrite(PIN_STEP,         LOW);
}

void driveSelect(boolean enable) {
  digitalWrite(PIN_DRIVE_SELECT, enable ? LOW : HIGH);
}

void setHeadDirection(int headDirection) {
  currentHeadDirection = headDirection;
  digitalWrite(PIN_DIRECTION, headDirection == FORWARD ? LOW : HIGH);
}

void toggleStep() {
  // Switch head direction if end has been reached
  if (currentHeadPosition >= 2 * MAX_STEP_POSITION)
    setHeadDirection(BACKWARD);
  else if (currentHeadPosition <= 0)
    setHeadDirection(FORWARD);
    
  currentHeadPosition = currentHeadDirection == FORWARD ? currentHeadPosition + 1 : currentHeadPosition - 1;
  
  // Pulse the step pin
  digitalWrite(PIN_STEP, currentStepPinState ? LOW : HIGH);  
  currentStepPinState = ~currentStepPinState;
}

// Stops playing of current note and set read/write head back to track 0.
void resetDrive() {
  mute(); // Stop playing tone
  
  // Set read / write head to track 0
  driveSelect(true);
  for (uint8_t i = 0; i < 80; i++) { // For max drive's position
    setHeadDirection(BACKWARD);
    digitalWrite(PIN_STEP, HIGH);
    delayMicroseconds(1500);
    digitalWrite(PIN_STEP, LOW);
    delayMicroseconds(1500);
  }
  driveSelect(false);

  currentHeadPosition = 0; // We're reset.
  setHeadDirection(FORWARD); // Ready to go forward.
}

void floppyM0duleTick() {
  if (currentPeriod > 0) {
    if(TCNT1 >= currentPeriod) {
      TCNT1 = 0;
      toggleStep();
    }
  }
}

// Note: The minimum resolution for micros() is 4us.
void playTone(uint16_t period) {  
  currentPeriod = period; 
  driveSelect(currentPeriod ? true : false);
  TCNT1 = 0;
}

void mute() {
  playTone(0);
}
