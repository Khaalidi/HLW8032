#ifndef HLW8032_h
#define HLW8032_h


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SoftwareSerial.h>

class HLW8032
{
	public:
		HLW8032();
		void begin(HardwareSerial& SerialData,byte IO);
		void setVF(float Data);   //写入电压系数
		void setCF(float Data) ;   //写入电流系数
		void SerialReadLoop();  //串口循环业务 获取数据，并且解码数据
		float GetVol();   // 获取电压
		float GetVolAnalog();   // 获取电压原始ADC值
		float GetCurrent();  //获取电流
		float GetCurrentAnalog();  //获取电流原始ADC值
		float GetActivePower();   //获取有功功率
		float GetInspectingPower();  //获取视在功率
		float GetPowerFactor();   //获取功率因数
		uint16_t GetPF();   //获取电脉冲计数器
		uint32_t GetPFAll();   //获取总脉冲数
		float GetKWh();  // 获取累积电量
		
		uint32_t CurrentPar; 		//电流参数
		uint32_t CurrentData; 		//电流数据
		byte SerialTemps[24] ;  //串口缓冲区
		byte SeriaDataLen =0;  //数据长度计数器
		bool SerialRead = 0;  //串口数据OK标记
		
	private:
	
		bool Checksum();   //校验函数
		
		byte _IO;
		HardwareSerial *SerialID;
		uint8_t SysStatus;  //系统状态寄存器
		uint32_t VolPar;   //电压参数
		uint32_t VolData;   //电压数据
		float VF;   //电压系数
		
		float CF;             //电流系数
		uint32_t PowerPar;         //功率参数
		uint32_t PowerData;        //功率数据
		uint16_t PF;               //脉冲计数器
		uint32_t PFData = 1;           //脉冲溢出计数器
		uint32_t VolR1 = 1880000;    //电压电阻1 470K*4  1880K
		uint32_t VolR2 = 1000;        //电压电阻2  1K
		float CurrentRF = 0.1;       // 电流采样电阻 0.1 欧姆
};


#endif