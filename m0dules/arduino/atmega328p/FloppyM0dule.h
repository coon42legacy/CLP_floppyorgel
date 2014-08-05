#include <Arduino.h>

void playMidiNote(uint16_t midi_note);
void resetDrive();
void tick();
static void driveSelect(boolean enable);
static void setDirection(int headDirection);
static void toggleStep(uint8_t step_pin, uint8_t direction_pin);

#define FORWARD LOW
#define BACKWARD HIGH

/*An array of maximum track positions for each step-control pin. 3.5" Floppies have
 80 tracks, 5.25" have 50.  These should be doubled, because each tick is now
 half a position (use 158 and 98).
 When using all possible tracks, sometimes the head will make a clicking noise, when reaching the end.
 To avoiding damage of the head, there should be some space
 Note: This seems to be a race condition of the interrupt. Thats why the interrupt got replaced with a polling
       solution.
*/
 

static uint8_t currentPosition     = 0;
static uint8_t currentDirection    = FORWARD;
static uint8_t currentStepPinState = LOW;

//Current period assigned to each pin.  0 = off.  Each period is of the length specified by the RESOLUTION
//variable above.  i.e. A period of 40 is (RESOLUTION x 40) microseconds long.
static uint16_t currentPeriod = 0;
static uint16_t currentTick   = 0;

// todo: calulate from RESOLUTION setting. (40ms here)
static uint16_t const microPeriods[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  30578, 28861, 27242, 25713, 24270, 22909, 21622, 20409, 19263, 18182, 17161, 16198, // C1 - B1
  15289, 14436, 13621, 12856, 12135, 11454, 10811, 10205, 9632, 9091, 8581, 8099,     // C2 - B2
  7645, 7218, 6811, 6428, 6068, 5727, 5406, 5103, 4816, 4546, 4291, 4050,             // C3 - B3
  3823, 3609, 3406, 3214, 3034, 2864, 2703, 2552, 2408, 2273, 2146, 2025,             // C4 - B4
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
