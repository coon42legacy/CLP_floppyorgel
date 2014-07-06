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
// 
// UTFT(byte model, int RS, int WR,int CS, int RST, int SER=0);
// RS: PL2
// WR: PL1
// LCD_CS: PA_4
// RESET: PL0 
// D0  - D7:  PK_0 - PK_7
// D8  - D11  PE_0 - PE_3
// D12 - D15: PE_4 - PE_7

// SD-Pinout:
// ==========
// SD_CS: PA_5

#include "UTFT.h"
#include "SD.h"

File root;

// Declare which fonts we will be using
extern uint8_t SmallFont[];

UTFT  myGLCD(SSD1289, PL_2, PL_1, PL_3, PL_0);   // Remember to change the model parameter to suit your display module!

void setup()
{
  randomSeed(analogRead(0));
  
// Setup the LCD
  myGLCD.InitLCD();
  myGLCD.setFont(SmallFont);
  
  pinMode(USR_SW1, INPUT); // UP
  pinMode(USR_SW2, INPUT); // DOWN
  pinMode(PA_4, OUTPUT); // LCD Chip Select
  pinMode(PA_5, OUTPUT); // SD Chip Select
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
}

void drawMenu() {
  //myGLCD.fillRect(4, 20, 315, 30);
}

void loop()
{
  int buf[318];
  int x, x2;
  int y, y2;
  int r;

// Clear the screen and draw the frame
  myGLCD.clrScr();
  drawFrame();
  drawMenu();
  
  Serial.begin(115200); 
  Serial.print("Initializing SD card...");

  if (!SD.begin(PA_5, SPI_HALF_SPEED, 2)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  root = SD.open("/");
  printDirectory(root, 0);

  Serial.println("done!");
   
  while(true);
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
