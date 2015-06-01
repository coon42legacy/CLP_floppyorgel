// Floppy Controller for Floppy Orgel v3.0
// by coon <coon@c-base.org>

// Implements a controller based on a Atmel 328p with
// Arduino bootloader.
// This version uses CTC mode for the stepper motor and
// SPI with a 595 shift register for some status LEDs.

// Created 30 July 2014
// Last edit: 01 June 2015

#define BUILD_VER "2.0"
#include <SPI.h>
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
  digitalWrite(PIN_RS485_DE,  LOW); // Enable bus input
  digitalWrite(PIN_RS485_NRE, LOW); // Disable bus output
}

void rs485sendMode() {
  digitalWrite(PIN_RS485_DE,  HIGH); // Disable bus input
  digitalWrite(PIN_RS485_NRE, HIGH); // Enable bus output
}

void initRS485() {
  pinMode(PIN_RS485_DE,  OUTPUT); // Driver Outputs Enable
  pinMode(PIN_RS485_NRE, OUTPUT); // Receive Output Enable
  rs485listenMode();
}

void sendLedStatus(byte data) {
  digitalWrite(PIN_SHIFT_REGISTER_RCLK, LOW);
  SPI.transfer(data);
  digitalWrite(PIN_SHIFT_REGISTER_RCLK, HIGH);
}

void initStatusLeds() {
  pinMode(PIN_SHIFT_REGISTER_RCLK, OUTPUT);
  pinMode(A3, OUTPUT); // obsolete on v2.1
  digitalWrite(A3, LOW); // obsolete on v2.1
  digitalWrite(PIN_SHIFT_REGISTER_RCLK, HIGH);
  SPI.begin();
  sendLedStatus(0x00);

  for(int i = 0; i < 6; i++) {
    sendLedStatus(1 << i);
    delay(50);
  }
}

void initUART() {
  Serial.begin(9600); // Start serial for output
}

uint8_t getFloppyAddress() {
  return (~PIND >> 2) & 0x3F;
}

void setup() {
  initDipSwitch();
  initStatusLeds();
  initFloppyM0dule();
  initUART();
  initRS485();

  Serial.println("resetting drive...");
  resetDrive();

  sendLedStatus(0x01);
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
  checkDipSwitchChange(); 
  checkRS485data();
}

void checkRS485data() {  
  static uint8_t midi_channel = 0;
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
        if(Serial.available() > 2) {
          midi_channel = Serial.read();
          period_high = Serial.read();
          period_low = Serial.read();
          
          if(midi_channel == getFloppyAddress())
            playTone(period_high << 8 | period_low); 
          
          syncState = 0;
        }
        break;
    }
  }
}
