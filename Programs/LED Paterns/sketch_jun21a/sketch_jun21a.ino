#include <TimerOne.h>
#include "LPD6803.h"
//#include "LPD8806.h"
//#include "SPI.h

int dataPin = 11;       // 'green' wire
int clockPin = 13;      // 'blue' wire
//int c= Color(0,63,0);
LPD6803 strip = LPD6803(99, dataPin, clockPin);

int r = 6;
int g = 2;
int b = 25;
void setup() {
  strip.setCPUmax(50);
  
  strip.begin();
  
  strip.show();
  
  int i;
  
  //cool : Color(5,63,20))
  
  for(i=0;i<strip.numPixels();i++)
  {
    strip.setPixelColor(i,(Color(0,0,255)));
    strip.show();
  }
  //colorWipe(strip.(127, 127, 127),50);

}

void loop() {
  // put your main code here, to run repeatedly:

}
unsigned int Color(byte r, byte g, byte b)
{
  //Take the lowest 5 bits of each value and append them end to end
  return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
}

void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}
