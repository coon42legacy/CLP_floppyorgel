// Floppy Controller for Floppy Orgel v3.0
// by coon <coon@c-base.org>

// Implements a controller based on a Atmel 328p with
// Arduino bootloader.

// Created 30 July 2014
// Last edit: 14 August 2014

#define BUILD_VER "0.6"
#include <WSWire.h>
#include <avr/wdt.h>
#include "FloppyM0dule.h"


void initDipSwitch() {
  pinMode(2, INPUT_PULLUP); // 1
  pinMode(3, INPUT_PULLUP); // 2
  pinMode(4, INPUT_PULLUP); // 3
  pinMode(5, INPUT_PULLUP); // 4
  pinMode(6, INPUT_PULLUP); // 5
  pinMode(7, INPUT_PULLUP); // 6
}

void initI2C() {
  Wire.begin(getFloppyAddress()); // Join I2C bus on address set on DIP switch.
  Wire.onReceive(onReceiveI2C); // Register I2C event callback
}

void initUART() {
  Serial.begin(115200); // start serial for output
}

uint8_t getFloppyAddress() {
  return (~PIND >> 2) & 0x3F;
}

void setup() {
  initDipSwitch();
  initFloppyM0dule();
  initUART();
  initI2C();
  resetDrive();
  //wdt_enable(WDTO_2S); // if the atmega hangs, it will reboot itself after 2 seconds 
  
  Serial.print("Floppy controller initialized with MIDI channel ");
  Serial.print(getFloppyAddress(), DEC);
  Serial.println(". (Build Version: " BUILD_VER ")");  
}

void checkDipSwitchChange() {
  static uint8_t my_old_address = getFloppyAddress();
  if(getFloppyAddress() != my_old_address) {
    Serial.print("Address changed from ");
    Serial.print(my_old_address, DEC);
    Serial.print(" to ");
    Serial.println(getFloppyAddress(), DEC);
    my_old_address = getFloppyAddress();
    Wire.begin(getFloppyAddress()); // Change I2C address
    playTone(0);
  }
}

void loop()
{
  tick();
  checkDipSwitchChange(); 
  
  //wdt_reset(); // watchdog reset
}

void onReceiveI2C(int numBytes)
{ 
  while(Wire.available())
  {    
    uint8_t period_high = Wire.read();
    uint8_t period_low = Wire.read();
    uint16_t period = period_high << 8 | period_low;
    playTone(1000000 / period);
    
    Serial.print("Playing frequency: ");
    Serial.println(period = period > 0 ? 500000 / period : 0);
  }
}
