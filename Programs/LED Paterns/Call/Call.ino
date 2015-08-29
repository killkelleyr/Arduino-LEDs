#include <TimerOne.h>
#include "LPD6803.h"
#include "SPI.h"

int dataPin = 11;       // 'green' wire
int clockPin = 13;      // 'blue' wire
     
uint8_t LED_Breathe_Table[]  = {   80,  87,  95, 103, 112, 121, 131, 141, 151, 161, 172, 182, 192, 202, 211, 220,
              228, 236, 242, 247, 251, 254, 255, 255, 254, 251, 247, 242, 236, 228, 220, 211,
              202, 192, 182, 172, 161, 151, 141, 131, 121, 112, 103,  95,  87,  80,  73,  66,
               60,  55,  50,  45,  41,  38,  34,  31,  28,  26,  24,  22,  20,  20,  20,  20,
               20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,
               20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  20,  22,  24,  26,  28,
               31,  34,  38,  41,  45,  50,  55,  60,  66,  73 };
     
#define BREATHE_TABLE_SIZE (sizeof(LED_Breathe_Table))
#define BREATHE_CYCLE    6000      /*breathe cycle in milliseconds*/
#define BREATHE_UPDATE    (BREATHE_CYCLE / BREATHE_TABLE_SIZE)     
int cycle;     
     
int nLEDs = 50;
            /*  Change to number of LEDs  */
            /*          |                 */
LPD6803 strip = LPD6803(nLEDs, dataPin, clockPin);
#define COLOR(r, g, b)

//char *functionnames[]{"Rainbow","Wipe"};
int funct;
long chosen;
int count=1;
//#define numfun 3

void setup() {
  
  strip.setCPUmax(80); //Set the CPU Usage
  Serial.begin(9600);
  randomSeed(9001);
  strip.begin(); //Initiate Strip
  strip.show(); //Turn all LEDs off
  
}

void loop() {
  //wipe(50);
  //delay(10000);
  rainbow(50);
  //chosen = random(sizeof(functionnames)/sizeof(char*));
  /*chosen = random(1,4);
  Serial.println(chosen);
  switch(chosen){
    case 1:
      rainbow(50);
      break;
    case 2:
      wipe(50);
      break;
    case 3:
      {
        for (cycle=0; cycle < 4; cycle++) {
    uniformBreathe(LED_Breathe_Table, BREATHE_TABLE_SIZE, BREATHE_UPDATE, 127, 127, 127);
  }
  for (cycle=0; cycle < 4; cycle++) {
    sequencedBreathe(LED_Breathe_Table, BREATHE_TABLE_SIZE, BREATHE_UPDATE, 127, 127, 127);
  }
      }
      break;
  }
  delay(1000);*/
  //rainbow(50);
  //rainbowCycle(50);

}

void uniformBreathe(uint8_t* breatheTable, uint8_t breatheTableSize, uint16_t updatePeriod, uint16_t r, uint16_t g, uint16_t b)
{
  int i;
  uint8_t breatheIndex = 0;
  uint8_t breatheRed;
  uint8_t breatheGrn;
  uint8_t breatheBlu;
  
  for (breatheIndex = 0; breatheIndex < breatheTableSize; breatheIndex++) {
    for (i=0; i < strip.numPixels(); i++) {
      breatheRed = (r * breatheTable[breatheIndex]) / 256;
      breatheGrn = (g * breatheTable[breatheIndex]) / 256;
      breatheBlu = (b * breatheTable[breatheIndex]) / 256;
      strip.setPixelColor(i, breatheRed, breatheGrn, breatheBlu);
    }
    strip.show();   // write all the pixels out
    delay(updatePeriod);
  }
}

void sequencedBreathe(uint8_t* breatheTable, uint8_t breatheTableSize, uint16_t updatePeriod, uint16_t r, uint16_t g, uint16_t b)
{
  int i;
  uint8_t breatheIndex = 0;
  uint8_t breatheRed;
  uint8_t breatheGrn;
  uint8_t breatheBlu;
  uint8_t sequenceIndex;
  
  for (breatheIndex = 0; breatheIndex < breatheTableSize; breatheIndex++) {
    for (i=0; i < strip.numPixels(); i++) {
      sequenceIndex = (breatheIndex + (i*4)) % breatheTableSize;
      breatheRed = (r * breatheTable[sequenceIndex]) / 256;
      breatheGrn = (g * breatheTable[sequenceIndex]) / 256;
      breatheBlu = (b * breatheTable[sequenceIndex]) / 256;
      strip.setPixelColor(i, breatheRed, breatheGrn, breatheBlu);
    }
    strip.show();   // write all the pixels out
    delay(updatePeriod);
  }
}

void wipe(uint8_t wait){
  {
  colorWipe(Color(63, 0, 0), 50);
  colorWipe(Color(0, 63, 0), 50);
  colorWipe(Color(0, 0, 63), 50);
  count++;
  }//while(count<100);
}
void colorWipe(uint16_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
void rainbow(uint8_t wait){
  int i,j;
  {
   for (j=0; j < 96 * 1; j++) {     // 3 cycles of all 96 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 96));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
  //count++;
  }//while(count<100);
}

void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 96 * 5; j++) {     // 5 cycles of all 96 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 96 / strip.numPixels()) + j) % 96) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

/* Helper functions */

// Create a 15 bit color value from R,G,B
unsigned int Color(byte r, byte g, byte b)
{
  //Take the lowest 5 bits of each value and append them end to end
  return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
}

//Input a value 0 to 127 to get a color value.
//The colours are a transition r - g -b - back to r
unsigned int Wheel(byte WheelPos)
{
  byte r,g,b;
  switch(WheelPos >> 5)
  {
    case 0:
      r=31- WheelPos % 32;   //Red down
      g=WheelPos % 32;      // Green up
      b=0;                  //blue off
      break; 
    case 1:
      g=31- WheelPos % 32;  //green down
      b=WheelPos % 32;      //blue up
      r=0;                  //red off
      break; 
    case 2:
      b=31- WheelPos % 32;  //blue down 
      r=WheelPos % 32;      //red up
      g=0;                  //green off
      break; 
  }
  return(Color(r,g,b));
}

