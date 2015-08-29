/*
 * arduino serial-i2c-gateway, Copyright (C) 2011 michael vogt <michu@neophob.com>
 *  
 * This file is part of neorainbowduino.
 *
 * neorainbowduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * neorainbowduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Iwnc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 	
 */

#include <TimerOne.h>
#include "LPD6803.h"

#define PIXELS 20

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
int dataPin = 2;       // 'green' wire
int clockPin = 3;      // 'blue' wire
// Don't forget to connect 'blue' to ground and 'red' to +5V

byte kr=0;
byte krDirection=0;

//initialize strip with 20 leds
LPD6803 strip = LPD6803(PIXELS, dataPin, clockPin);

void setup() {
  pinMode(13, OUTPUT);
  randomSeed(analogRead(0));

  strip.setCPUmax(50);  // start with 50% CPU usage. up this if the strand flickers or is slow

  // Start up the LED counter
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();
}

 
// Create a 15 bit color value from R,G,B
unsigned int Color(byte r, byte g, byte b) {
  //Take the lowest 5 bits of each value and append them end to end
  return( ((unsigned int)r & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)g & 0x1F);
}


void knightRider() {
  for (byte i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }

  if (kr<PIXELS-1) {
    strip.setPixelColor(kr+1, Color(10, 0, 0));
  }
  strip.setPixelColor(kr, Color(31, 0, 0));
  if (kr>1) {
    strip.setPixelColor(kr-1, Color(10, 0, 0));
  }
 
  //swap buffers asap, all 20 pixels
  strip.doSwapBuffersAsap(PIXELS);

  delay(30);

  if (krDirection==0) {
    kr++;
  } else {
    kr--;
  }
  
  if (kr>PIXELS-1) {
    krDirection = 1;
    kr = PIXELS;
  }
  
  if (kr==0) {
   krDirection = 0;
   kr = 0; 
  }
}


void loop() {
  knightRider();
}


