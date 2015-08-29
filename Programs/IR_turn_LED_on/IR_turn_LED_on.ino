/*
This code will turn an led on and off based on the input from an IR Remote

IR Sensor
Vout to Pin 11 on arduino
GND to GND on arduino
VCC to +5V on arduino

*/

#include <IRremote.h>

int IRpin = 11;
int LED = 12;
IRrecv irrecv(IRpin);
decode_results results;

boolean LEDon = true; //Sets LEDon as true

void setup() {
  
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start IR Receiver
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  
}

void loop() {
  
   if (irrecv.decode(&results)) 
    {
      //Serial.println(results.value, DEC);
      irrecv.resume();   // Receive the next value
    }
  
  switch(results.value){
    case 2819522142:
          switch(LEDon){
                case true:
                      LEDon = false;
                      digitalWrite(LED, LOW);
                      //delay(200);
                      break;
                case false:
                      LEDon = true;
                      digitalWrite(LED, HIGH);
                      //delay(200);
                      break;
          }
          break;
  }
   /*if (results.value == 2819522142)  
     {
       if (LEDon == true)   // is LEDon equal to true? 
         {
           LEDon = false;   
           digitalWrite(LED, HIGH);
           delay(100);      // keeps the transistion smooth
           break; 
         }
         
        else
          {
            LEDon = true;
            digitalWrite(LED, LOW);
            delay(100);
            break;
          }*/
    //}
}
