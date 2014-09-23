// UTFT_Demo_320x240 (C)2012 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of how to use most of the functions
// of the library with a supported display modules.
//
// This demo was made for modules with a screen resolution 
// of 320x240 pixels.
//
// This program requires the UTFT library.
//

// LCD-Pinout:
// ===========
// RS: PL2
// WR: PL1
// LCD_CS: PL_3
// RESET: PL0 
// D0  - D7:  PK_0 - PK_7
// D8  - D11  PE_0 - PE_3
// D12 - D15: PE_4 - PE_7

// I2C-Pinout:
// ===========
// SDA: PN_4
// SCL: PN_5

// SD-Pinout:
// ==========
// SD_CS: PL_4
// MOSI:  PE_4
// MISO:  PE_5
// SCK:   PB_5

// GamePad-Pinout:
// ===============
//  Data:  PM_0
//  Latch: PM_1 
//  Clock: PM_2 

#include "UTFT.h"
#include "SD.h"
#include "Wire_hotfix.h"
#include "NesGamePad.h"


File root;

// Declare which fonts we will be using
extern uint8_t SmallFont[];

UTFT myGLCD(SSD1289, PL_2, PL_1, PL_3, PL_0); // Remember to change the model parameter to suit your display module!

void setupUart() {
  Serial.begin(115200);
}

void setupLcd() {
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  drawMenu();
}

void setupButtons() {
  pinMode(USR_SW1, INPUT); // UP
  pinMode(USR_SW2, INPUT); // DOWN
}

void setupSdCard() {
  pinMode(PL_4, OUTPUT); // SD Chip Select
  Serial.print("Initializing SD card...");

  if (SD.begin(PL_4, SPI_HALF_SPEED, 1)) {
    Serial.println("initialization done.");
    root = SD.open("/");
    printDirectory(root, 0);
  
    Serial.println("done!");
  }
  else {
    Serial.println("Initialization failed!");
    myGLCD.setColor(255,255,0);
    myGLCD.print("NO SD CARD!", CENTER, 240 / 2);
  }
}

void setupI2c() {
  Wire.begin(); // join i2c bus (address optional for master)
}

void setup()
{  
  //randomSeed(analogRead(0));
  setupUart();
  setupLcd();
  setupButtons();
  setupI2c();
  setupSdCard();
}

void drawFrame() {
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 226, 319, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("Floppy Orgel v3.0", CENTER, 1);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255,255,0);
  myGLCD.print("coon@c-base.org", CENTER, 227);

  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRect(0, 14, 319, 225);  
  myGLCD.setColor(255,255,0);
}

void drawMenu() {
  myGLCD.clrScr();
  drawFrame();
}

void loop()
{
  setupGamePad();
  
  while(true) {
    NesGamePadStates_t state = getGamepadState();
    
    /*
    Serial.print("State: 0x"); 
    if(state.code < 0x10) Serial.print("0");
    Serial.println(state.code, HEX);
    */
    
    Serial.print("A: ");      if(state.states.A)      Serial.print(" ON"); else Serial.print("OFF"); Serial.print(" | ");
    Serial.print("B: ");      if(state.states.B)      Serial.print(" ON"); else Serial.print("OFF"); Serial.print(" | ");
    Serial.print("UP: ");     if(state.states.North)  Serial.print(" ON"); else Serial.print("OFF"); Serial.print(" | ");
    Serial.print("DOWN: ");   if(state.states.South)  Serial.print(" ON"); else Serial.print("OFF"); Serial.print(" | ");
    Serial.print("LEFT: ");   if(state.states.West)   Serial.print(" ON"); else Serial.print("OFF"); Serial.print(" | ");
    Serial.print("RIGHT: ");  if(state.states.East)   Serial.print(" ON"); else Serial.print("OFF"); Serial.print(" | ");
    Serial.print("START: ");  if(state.states.Start)  Serial.print(" ON"); else Serial.print("OFF"); Serial.print(" | ");
    Serial.print("SELECT: "); if(state.states.Select) Serial.print(" ON"); else Serial.print("OFF"); Serial.print(" | ");
    Serial.println("");
    
    
    delay(50);
  }
  
  uint8_t midi_channel = 0;
  uint8_t period_low = 0;
  uint8_t period_high = 0;
  uint16_t period = 0;
  Serial.println("Now waiting for floppy data...");

  uint8_t syncState = 0;
  uint8_t syncErrors = 0;
  uint32_t lastTime = millis();
  
  while(true) {
    /*
    if(millis() - lastTime > 250) {
      digitalWrite(RED_LED, !digitalRead(RED_LED));
      lastTime = millis();
    }
    */
    
    if (Serial.available()) {
      switch(syncState) {
        case 0:
          if(Serial.read() == 0x55) 
            syncState = 1;
          else {
            syncState = 0;
            
            myGLCD.clrScr();
            myGLCD.print("Sync Errors: ", CENTER, 120);
            myGLCD.printNumI(++syncErrors, CENTER, 130);
          } 
          break;
        
        case 1:
          if(Serial.read() == 0xAA) 
            syncState = 2;
          else {
            syncState = 0;
            myGLCD.clrScr();
            myGLCD.print("Sync Errors: ", CENTER, 120);
            myGLCD.printNumI(++syncErrors, CENTER, 130);
          } 
        break;
              
        case 2:
          if(Serial.available() > 2) {
            midi_channel = Serial.read();
            period_high = Serial.read();
            period_low = Serial.read();
            
            Wire.beginTransmission(midi_channel);
            Wire.write(0x55);
            Wire.write(0xAA);
            Wire.write(period_high);
            Wire.write(period_low);
            Wire.endTransmission();
            
            syncState = 0;
            /*
            myGLCD.clrScr();
            myGLCD.print("On Sync.", CENTER, 120);
            */
          }
        break;
      }
    }
  }
}

void printDirectory(File dir, int numTabs) {
  int y_pos = 17;
  
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     
     // display
     myGLCD.setColor(255,255,0);
     myGLCD.print(entry.name(), 5, y_pos);
     y_pos += 13;
     
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}