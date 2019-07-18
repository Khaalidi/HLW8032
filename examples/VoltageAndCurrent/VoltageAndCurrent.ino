#include "HLW8032.h"
#define SYNC_TIME 500

HLW8032 HL;


static unsigned long start_time, end_time;

void setup()
{

HL.begin(Serial1,4);   
Serial.begin(9600);
Serial.println("[HLW8032] Ready! ...");  

start_time = millis();
}


void loop()
{
  end_time = millis();
	HL.SerialReadLoop();

 if (end_time - start_time > SYNC_TIME)
  {
    if(HL.SerialRead == 1)   
    {
      Serial.print("Voltage: ");
      Serial.print(HL.GetVol()*0.001);
      Serial.print(" .      Current: ");
      Serial.print(HL.GetCurrent());
      Serial.println();
 
    }
    start_time = end_time;
  }

}
