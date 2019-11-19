#include "HLW8032.h"
//mega2560 examples   
HLW8032 HL;

void setup()
{
	HL.begin(Serial1,4);  
	Serial.begin(9600); 
}

void loop()
{
	HL.SerialReadLoop();    
  
	if(HL.SerialRead == 1)   
	{
		Serial.println(HL.GetVol());  
	}
}