#include "HLW8032.h"

HLW8032::HLW8032()
{
}

void HLW8032::begin(HardwareSerial& SerialData,byte IO)
{
	 _IO = IO;
	 pinMode(_IO,OUTPUT);
	 digitalWrite(_IO,LOW);
	 delay(10);
	 SerialID = &SerialData; 
	 SerialID->begin(4800,SERIAL_8E1);   
	 //while(SerialID->read()>= 0){}
	 digitalWrite(_IO,HIGH);
	 
	 VF = VolR1 / VolR2 ;  
	 CF = 1.0 / (CurrentRF *1000.0);    
}


void HLW8032::setVF(float Data)
{
	VF = Data;
}



void HLW8032::setCF(float Data)
{
	CF = Data;
}

void HLW8032::SerialReadLoop()
{
	if (SerialID->available()>0)   
	{
		delay(56);
		SeriaDataLen = SerialID->available();
		
		if (SeriaDataLen !=24)
		{
			while(SerialID->read()>= 0){}
			//return;
		}

		for (byte a = 0; a < SeriaDataLen; a++)  
		{
			SerialTemps[a] =  SerialID->read();
		}
		//Serial.println(SerialID->available());
		


		if(SerialTemps[1] != 0x5A) 
		{
			while(SerialID->read()>= 0){}
			return;
		}
		if(Checksum() == false)   
		{
			//Serial.println("crc error");
			return;
		}
		

		SerialRead = 1;  
		VolPar = ((uint32_t)SerialTemps[2]  <<16) + ((uint32_t)SerialTemps[3] <<8) + SerialTemps[4]; 
		if(bitRead(SerialTemps[20], 6) == 1)  
		{
			VolData = ((uint32_t)SerialTemps[5]  <<16) + ((uint32_t)SerialTemps[6] <<8) + SerialTemps[7];
		}
		CurrentPar = ((uint32_t)SerialTemps[8]  <<16) + ((uint32_t)SerialTemps[9] <<8) + SerialTemps[10];  
		if(bitRead(SerialTemps[20], 5) == 1)   
		{
			CurrentData = ((uint32_t)SerialTemps[11]  <<16) + ((uint32_t)SerialTemps[12] <<8) + SerialTemps[13];  
		}
		PowerPar = ((uint32_t)SerialTemps[14]  <<16) + ((uint32_t)SerialTemps[15] <<8) + SerialTemps[16];   
		if(bitRead(SerialTemps[20], 4) == 1)   
		{
			PowerData = ((uint32_t)SerialTemps[17]  <<16) + ((uint32_t)SerialTemps[18] <<8) + SerialTemps[19];    
		}
		PF = ((uint32_t)SerialTemps[21] <<8) + SerialTemps[22];     
		

		if(bitRead(SerialTemps[20], 7) == 1)
		{
			PFData++;
		}
	}
}




float HLW8032::GetVol()
{
	float Vol = (VolPar / VolData) * VF;   
	return Vol;
} 


float HLW8032::GetVolAnalog()
{
	float Vol = VolPar / VolData;
	return Vol;
}


float HLW8032::GetCurrent()
{
	float Current = (CurrentPar / CurrentData) * CF;    
	return Current;
}


float HLW8032::GetCurrentAnalog()
{
	float Current  = CurrentPar / CurrentData;
	return Current;
}



float HLW8032::GetActivePower()
{
	float Power = (PowerPar/PowerData) * VF * CF;  
	return Power;
}


float HLW8032::GetInspectingPower()
{
	float vol = GetVol();
	float current = GetCurrent();
	return vol* current;
}


float HLW8032::GetPowerFactor()
{
	float ActivePower = GetActivePower();   
	float InspectingPower = GetInspectingPower(); 
	return ActivePower / InspectingPower ;  
}



uint16_t HLW8032::GetPF()
{
	return PF;
}



uint32_t HLW8032::GetPFAll()
{
	return PFData * PF;
}



float HLW8032::GetKWh()
{
	float InspectingPower = GetInspectingPower();
	uint32_t PFcnt = (1/PowerPar) *(1/InspectingPower) * 1000000000 * 3600;  
	float KWh = (PFData * PF) / PFcnt;  
	return KWh;

}



bool HLW8032::Checksum()
{
	byte check = 0;
	for(byte a = 2;a<=22;a++)
	{
		check = check + SerialTemps[a];
	}
	if (check  == SerialTemps[23])
	{

		return true;
	}
	else
	{
		return false;  
	}
}