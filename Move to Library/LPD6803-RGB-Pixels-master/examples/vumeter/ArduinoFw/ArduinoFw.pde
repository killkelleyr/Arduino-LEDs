/*
 * arduino serial-led-gateway, Copyright (C) 2011 michael vogt <michu@neophob.com>
 * get some led at http://www.pixelinvaders.ch
 *  
 * This file is part of PixelController.
 *
 * PixelController is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * PixelController is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * 	
 */

#include <TimerOne.h>  
#include "LPD6803.h"

#define BAUD_RATE 115200
#define CMD_START_BYTE  0x01
#define START_OF_DATA 0x10 
#define END_OF_DATA 0x20

byte serInStr[3]; 	 				 // array that will hold the serial input string

// Choose which 2 pins you will use for output.
// Can be any valid output pins.
int dataPin = 2;       // 'green' wire
int clockPin = 3;      // 'blue' wire
// Don't forget to connect 'blue' to ground and 'red' to +5V

//initialize strip with 20 leds
LPD6803 strip = LPD6803(20, dataPin, clockPin);


unsigned int Color(byte r, byte g, byte b) {
  //Take the lowest 5 bits of each value and append them end to end
  return( ((unsigned int)g & 0x1F )<<10 | ((unsigned int)b & 0x1F)<<5 | (unsigned int)r & 0x1F);
}


//create initial image
void showInitImage() {
    for (int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
    }    
}


// --------------------------------------------
//      setup
// --------------------------------------------
void setup() {
  //im your slave and wait for your commands, master!
  Serial.begin(BAUD_RATE); //Setup high speed Serial
  Serial.flush();

  strip.setCPUmax(50);  // start with 50% CPU usage. up this if the strand flickers or is slow

  // Start up the LED counter
  strip.begin();
  
  showInitImage();
  
  // Update the strip, to start they are all 'off'
  strip.show();
}

// --------------------------------------------
//      loop
// --------------------------------------------
void loop() {
  readCommand();
}

//draw vu
void vu(byte val) {
  int c;
  val+=10;
  for (byte i=10; i < strip.numPixels(); i++) {
    if (val>i) {       
       if (i>17) c = Color(0, 31, 0);
       else
       if (i>15) c = Color(31, 31, 0);
       else
       c = Color(31, 0, 0);
    } else {
      c = 0;
    }
    strip.setPixelColor(i, c);      
  }  
}


/* 
 --------------------------------------------
     read serial command
 --------------------------------------------
 */
 
byte readCommand() {
  byte b,i,sendlen;

  //wait until we get a CMD_START_BYTE or queue is empty
  i=0;
  while (Serial.available()>0 && i==0) {
    b = Serial.read();
    if (b == CMD_START_BYTE) {
      i=1;
    }
  }

  if (i==0) {
    //failed to get data ignore it
    return 0;    
  }

  b=5;
  i=0;
  while (i<3) {
    if (Serial.available()) {
      serInStr[i++] = Serial.read();
    } else {
      delay(2); 
      if (b-- == 0) {
        return 0;        //no data available!
      }      
    }
  }
  
  //very basic sanity check
  if (serInStr[0]==serInStr[1]) {
    vu(serInStr[0]);
    strip.doSwapBuffersAsap(20);    
  }

  delay(20);
}


