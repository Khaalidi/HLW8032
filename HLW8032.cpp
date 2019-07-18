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
	 SerialID->begin(4800,SERIAL_8E1);   //指定4800波特率，偶校验  符号为->指针调用
	 //while(SerialID->read()>= 0){}
	 digitalWrite(_IO,HIGH);
	 
	 VF = VolR1 / VolR2 ;   //求电压系数
	 CF = 1.0 / (CurrentRF *1000.0);    //计算电流系数 
}

//写入电压系数,输入值为电压值，计算后得到修正值
void HLW8032::setVF(float Data)
{
	VF = Data;
}


//写入电流系数
void HLW8032::setCF(float Data)
{
	CF = Data;
}

void HLW8032::SerialReadLoop()
{
	if (SerialID->available()>0)   //检查串口是否有数据，并且缓冲区是否可用
	{
		delay(56);
		SeriaDataLen = SerialID->available();
		
		if (SeriaDataLen !=24)
		{
			while(SerialID->read()>= 0){}
			//return;
		}

		for (byte a = 0; a < SeriaDataLen; a++)  //获取所有字节数
		{
			SerialTemps[a] =  SerialID->read();
		}
		//Serial.println(SerialID->available());
		
		/*处理字节*/

		if(SerialTemps[1] != 0x5A)  //标记识别,如果不是就抛弃
		{
			while(SerialID->read()>= 0){}
			return;
		}
		if(Checksum() == false)   // 校验测试，如果错误就抛弃
		{
			//Serial.println("crc error");
			return;
		}
		
		//如果通过了以上测试，则说明数据包应该没问题，获取其中的数据
		SerialRead = 1;  // 数据包完备标记
		VolPar = ((uint32_t)SerialTemps[2]  <<16) + ((uint32_t)SerialTemps[3] <<8) + SerialTemps[4]; //获取电压参数寄存器
		if(bitRead(SerialTemps[20], 6) == 1)  //如果电压寄存器刷新，则取数据
		{
			VolData = ((uint32_t)SerialTemps[5]  <<16) + ((uint32_t)SerialTemps[6] <<8) + SerialTemps[7]; //获取电压寄存器
		}
		CurrentPar = ((uint32_t)SerialTemps[8]  <<16) + ((uint32_t)SerialTemps[9] <<8) + SerialTemps[10];  //电流参数 
		if(bitRead(SerialTemps[20], 5) == 1)   //如果电流寄存器更新，则取数据
		{
			CurrentData = ((uint32_t)SerialTemps[11]  <<16) + ((uint32_t)SerialTemps[12] <<8) + SerialTemps[13];  //电流
		}
		PowerPar = ((uint32_t)SerialTemps[14]  <<16) + ((uint32_t)SerialTemps[15] <<8) + SerialTemps[16];   // 功率参数
		if(bitRead(SerialTemps[20], 4) == 1)   // 如果功率寄存器数据更新，则取数据
		{
			PowerData = ((uint32_t)SerialTemps[17]  <<16) + ((uint32_t)SerialTemps[18] <<8) + SerialTemps[19];    //功率数据
		}
		PF = ((uint32_t)SerialTemps[21] <<8) + SerialTemps[22];   //脉冲数量寄存器       
		
		// 确认 PF进位寄存器是否进位，进位则添加1
		if(bitRead(SerialTemps[20], 7) == 1)
		{
			PFData++;
		}
	}
}



// 获取电压
float HLW8032::GetVol()
{
	float Vol = (VolPar / VolData) * VF;   //求电压有效值
	return Vol;
} 

//获取电压ADC值
float HLW8032::GetVolAnalog()
{
	float Vol = VolPar / VolData;
	return Vol; //返回厂商修正过的ADC电压值
}

//获取有效电流
float HLW8032::GetCurrent()
{
	float Current = (CurrentPar / CurrentData) * CF;    //计算有效电流
	return Current;
}

//获取电流厂商修正adc原始值
float HLW8032::GetCurrentAnalog()
{
	float Current  = CurrentPar / CurrentData;
	return Current;
}


//计算有功功率
float HLW8032::GetActivePower()
{
	float Power = (PowerPar/PowerData) * VF * CF;  // 求有功功率
	return Power;
}

//计算视在功率
float HLW8032::GetInspectingPower()
{
	float vol = GetVol();
	float current = GetCurrent();
	return vol* current;
}

//计算功率因数
float HLW8032::GetPowerFactor()
{
	float ActivePower = GetActivePower();   //获取有功功率
	float InspectingPower = GetInspectingPower(); //视在功率
	return ActivePower / InspectingPower ;  
}


//获取脉冲计数器值
uint16_t HLW8032::GetPF()
{
	return PF;
}


//获取总脉冲数
uint32_t HLW8032::GetPFAll()
{
	return PFData * PF;
}


//获取累积电量
float HLW8032::GetKWh()
{
	float InspectingPower = GetInspectingPower(); //视在功率
	uint32_t PFcnt = (1/PowerPar) *(1/InspectingPower) * 1000000000 * 3600;   //一度电的脉冲数量
	float KWh = (PFData * PF) / PFcnt;  //总脉冲除以1度电的脉冲量
	return KWh;

}


//校验测试
bool HLW8032::Checksum()
{
	byte check = 0;
	for(byte a = 2;a<=22;a++)
	{
		check = check + SerialTemps[a];
	}
	if (check  == SerialTemps[23])
	{
		//校验通过
		return true;
	}
	else
	{
		return false;  //校验不通过
	}
}