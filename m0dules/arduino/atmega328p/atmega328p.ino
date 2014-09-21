// Floppy Controller for Floppy Orgel v3.0
// by coon <coon@c-base.org>

// Implements a controller based on a Atmel 328p with
// Arduino bootloader.

// Created 30 July 2014
// Last edit: 21 September 2014

#define BUILD_VER "0.7"
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

void rs485listenMode() {
  digitalWrite(8, LOW); // Enable bus input
  digitalWrite(9, LOW); // // Disable bus output
}

void rs485sendMode() {
  digitalWrite(8, HIGH); // disable bus input
  digitalWrite(9, HIGH); // // enable bus output
}

void initRS485() {
  pinMode(8, OUTPUT); // PB0
  pinMode(9, OUTPUT); // PB1
  rs485listenMode();
}

void initUART() {
  Serial.begin(9600); // start serial for output
}

uint8_t getFloppyAddress() {
  return (~PIND >> 2) & 0x3F;
}

void initTimer1() {
  TCCR1A = 0x00; // normal operation page 148 (mode0);
  TCNT1  = 0x0000;  // set initial value to remove time error (16bit counter register)
  TCCR1B = 0x02; // start timer/ set clock
}

void setup() { 
  initTimer1();
  initDipSwitch();
  initFloppyM0dule();
  initUART();
  initRS485();
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
    playTone(0);
    mute();
  }
}

void loop() {
  floppyM0duleTick();
  checkDipSwitchChange(); 
  checkRS485data();
  //wdt_reset(); // watchdog reset
}

void checkRS485data() {  
  static uint8_t period_low = 0;
  static uint8_t period_high = 0;
  static uint8_t syncState = 0;
  static uint8_t syncErrors = 0;
  static boolean inErrorState = false;
  
  if(Serial.available()) {
    switch(syncState) {
      case 0:
        if(Serial.read() == 0x55) {
          syncState = 1;
          inErrorState = false;
        }
        else {
          syncState = 0;
          
          if(!inErrorState) {
            Serial.print("Sync Errors: ");     
            Serial.println(++syncErrors);
          }
          inErrorState = true;
        }
        break;
      
      case 1:
        if(Serial.read() == 0xAA)
          syncState = 2;
        else {
          syncState = 0;
          inErrorState = true;
          
          Serial.print("Sync Errors: ");
          Serial.println(++syncErrors);
        }
        break;
        
      case 2:
        if(Serial.available() > 1) {
          period_high = Serial.read();
          period_low = Serial.read();
          playTone(period_high << 8 | period_low); 
          
          syncState = 0;
        }
        break;
    }
  }
}
