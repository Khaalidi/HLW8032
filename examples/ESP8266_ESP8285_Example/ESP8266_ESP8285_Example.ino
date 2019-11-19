/*
ESP8266 and ESP8285 Example
*/

#include "HLW8032.h"

HLW8032 HL;

void setup()
{
    HL.begin(Serial1,4);  
    Serial.begin(4800);
    Serial.swap();
}

void loop()
{
  HL.SerialReadLoop();    

  if(HL.SerialRead == 1)   
  {
     Serial.println(HL.GetVol()); 
  }
}
