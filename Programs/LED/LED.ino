#include "LPD8806.h"
#include "SPI.h"

int nLEDs = 99;

int dataPin = 11;
int clockPin = 13;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  (strip.Color(127, 0, 0), 99); //Red
  (strip.Color(0,127, 0), 100);
  (strip.Color(0,0, 127), 100);
  (strip.Color(127,127, 127), 100);
}
