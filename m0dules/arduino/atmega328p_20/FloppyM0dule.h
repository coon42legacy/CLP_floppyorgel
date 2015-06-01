#include <Arduino.h>
#include "FloppyM0dule_conf.h"

#define FORWARD LOW
#define BACKWARD HIGH

void initFloppyM0dule();
void playTone(uint16_t frequency);
void mute();
void resetDrive();
void floppyM0duleTick();