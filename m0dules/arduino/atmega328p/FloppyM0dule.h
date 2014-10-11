#include <Arduino.h>
#include "FloppyM0dule_conf.h"

#define FORWARD LOW
#define BACKWARD HIGH

void initFloppyM0dule();
void playTone(uint16_t frequency);
void mute();
void resetDrive();
void floppyM0duleTick();
static void driveSelect(boolean enable);
static void setHeadDirection(int headDirection);
static void toggleStep();

static uint8_t currentHeadPosition  = 0;
static uint8_t currentHeadDirection = FORWARD;
static uint8_t currentStepPinState  = LOW;
static uint32_t currentPeriod = 0;
