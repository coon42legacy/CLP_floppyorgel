// Floppy Controller for Floppy Orgel v3.0
// by coon <coon@c-base.org>

// Implements a controller based on a Atmel 328p with
// Arduino bootloader.

// Created 30 July 2014

#define BUILD_VER "0.1"
#include <WSWire.h>
//#include <Wire.h>
#include "FloppyM0dule.h"

void initGPIO() {
  // DIP switch
  pinMode(2, INPUT); // 1
  pinMode(3, INPUT); // 2
  pinMode(4, INPUT); // 3
  pinMode(5, INPUT); // 4
  pinMode(6, INPUT); // 5
  pinMode(7, INPUT); // 6
  
  // Floppy
  pinMode(A0, OUTPUT); // Drive Select
  pinMode(A1, OUTPUT); // Direction
  pinMode(A2, OUTPUT); // Step
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
}

void initI2C() {
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
}

void initUART() {
  Serial.begin(115200);           // start serial for output
}

int getFloppyAddress() {
  return PIND >> 2;
}

void setup() {
  initGPIO();
  initUART();
  initI2C();
  resetDrive();
  
  Serial.print("Floppy controller initialized with MIDI channel ");
  Serial.print(getFloppyAddress(), DEC);
  Serial.println(". (Build Version: " BUILD_VER ")");
}

void loop()
{
  delayMicroseconds(40); // MOPPYEX_RESOLUTION
  tick();
  
  /*
  static byte my_address = getFloppyAddress();
  static byte my_old_address = my_address;
  
  // Debug
  if(my_address != my_old_address) {
    Serial.print("Address changed from ");
    Serial.print(my_old_address, DEC);
    Serial.print(" to ");
    Serial.println(my_address, DEC);
    my_old_address = my_address;
  }
  my_address = getFloppyAddress();
  */
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(Wire.available()) // loop through 
  {
    int c = Wire.read(); // receive midi note
    //playMidiNote(c);
    
    Serial.print("Playing midi note ");
    Serial.println(c);
  }
}
