#include "FloppyM0dule.h"

volatile uint8_t currentHeadDirection = FORWARD;
volatile uint8_t currentHeadPosition  = 0;

void initTimerOC1B() {
  pinMode (10, OUTPUT); // PB2 (OC1B)
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 0;
  TCCR1A |= (1 << COM1B0); // Toggle OC1B on Compare Match.
  TCCR1B |= (1 << WGM12); // set up timer with prescaler = 8 and CTC mode in stopped state
  TIMSK1 |= (1 << OCIE1B); // Enable TIMER1_COMPB interrupt on toggle
}

void initFloppyM0dule() {
  pinMode(PIN_DRIVE_SELECT, OUTPUT); 
  pinMode(PIN_DIRECTION,    OUTPUT); 
  pinMode(PIN_STEP,         OUTPUT);  
  digitalWrite(PIN_DRIVE_SELECT, LOW);
  digitalWrite(PIN_DIRECTION,    LOW);
  digitalWrite(PIN_STEP,         LOW);

  initTimerOC1B(); // for step pin toggeling
}

void driveSelect(boolean enable) {
  digitalWrite(PIN_DRIVE_SELECT, enable ? LOW : HIGH);
}

void setHeadDirection(int headDirection) {
  currentHeadDirection = headDirection;
  digitalWrite(PIN_DIRECTION, headDirection == FORWARD ? LOW : HIGH);
}

// on drive reset
ISR (TIMER1_COMPA_vect) {
  currentHeadPosition--; 
}

// on toggle step
ISR (TIMER1_COMPB_vect) { // Timer/Counter1 Compare Match B interrupt
  // Switch head direction if end has been reached
  if (currentHeadPosition >= 2 * MAX_STEP_POSITION)
    setHeadDirection(BACKWARD);
  else if (currentHeadPosition <= 0)
    setHeadDirection(FORWARD);
    
  currentHeadPosition = currentHeadDirection == FORWARD ? currentHeadPosition + 1 : currentHeadPosition - 1;
}

void enableRestMode(boolean enable) {
  if(enable) {
    setHeadDirection(BACKWARD);
    currentHeadPosition = 2 * MAX_STEP_POSITION;
    TIMSK1 &= ~(1 << OCIE1B); // Disable TIMER1_COMPB interrupt on toggle
    TIMSK1 |= (1 << OCIE1A); // Enable TIMER1_COMPA interrupt on toggle (Reset ISR)
  }
  else {
    setHeadDirection(FORWARD);
    TIMSK1 &= ~(1 << OCIE1A); // Disable TIMER1_COMPA interrupt on toggle (Reset ISR)
    TIMSK1 |= (1 << OCIE1B); // Enable TIMER1_COMPB interrupt on toggle
  }
}

// Stops playing of current note and set read/write head back to track 0.
void resetDrive() {  
  enableRestMode(true);
  
  driveSelect(true);  
  playTone(4000);
  while(currentHeadPosition); // wait until drive is reset...
  mute();
  driveSelect(false);

  enableRestMode(false);
}

void startStepToggle(uint16_t period) {
  OCR1A = period; // toggle pin after counting to period
  TCCR1B = (1 << WGM12) | (1 << CS11); // set up timer with prescaler = 8 and CTC mode
}

void stopStepToggle() {
  TCCR1B = (1 << WGM12); // stop timer (and therefore pin toggle)
}

void playTone(uint16_t period) {
  driveSelect(period ? true : false);
  
  if(period)
    startStepToggle(period);
  else
    stopStepToggle();
}

void mute() {
  playTone(0);
}
