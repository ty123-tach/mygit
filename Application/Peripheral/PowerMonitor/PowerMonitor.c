/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: PowerMonitor.c
	Author: ZhangZhiwei Version: V0 Date: 2019-09-02
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "PowerMonitor.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
#include "System.h"
#include "DrvTimer.h"
#include "DrvADC.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#define MAX_POWERADC_SAMPLE_FREQ	110	/*采样间隔，单位ms*/
#define MAX_POWERADC_TEMPSIZE		10	/*采样次数*/
static unsigned char gAdcPowerValueIndex = 0;
static unsigned short gAdcPowerValue = 0xFFFF;//电压采样值
static unsigned short gAdcTempPowerValue[MAX_POWERADC_TEMPSIZE];
static unsigned int gAdcPowerTimeStamp = 0;//电压采样时间戳
static float gSysPower;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
 
//**
// * @}
/*!
 * GetSystemPower
 * 获取系统电压
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
float GetSystemPower(void)
{
	return gSysPower;
}
/*!
 * GetPowerValue
 * 默认获取ADC值
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void GetPowerValue(unsigned short value)
{
	gAdcPowerValue = value;
}	
/*!
 * PowerMonitorInit
 * 电源监控初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void PowerMonitorInit(void)
{
    //物理芯片34号引脚作为模拟ADC功能
	DrvGpioSetPinMux(PORTA,PORTA_PIN1,PIN34_ADC0_SE1_ACMP0_IN1_TSI0_CH18);//PTA1 
	DrvAdcGetValueFuncRegister(ADCCH1_EXTFUNC_USE,GetPowerValue);
	gAdcPowerValue = 0xFFFF;
	gAdcPowerValueIndex = 0;
	gSysPower = 0;
}
/*!
 * PowerSystemAdcHandle
 * 电源系统ADC采样
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void PowerSystemAdcHandle(void)
{
	unsigned char lCnt;
	unsigned int Total = 0;
	float vol;
	if((DrvGetTimerMsTick() - gAdcPowerTimeStamp) < MAX_POWERADC_SAMPLE_FREQ)
		return;
	if(DrvAdcWorkStatus())
		return;
	gAdcPowerTimeStamp = DrvGetTimerMsTick();
	if(gAdcPowerValue != 0xFFFF){//采样结束
		gAdcTempPowerValue[gAdcPowerValueIndex++] = gAdcPowerValue;
		if(gAdcPowerValueIndex >= MAX_POWERADC_TEMPSIZE){//采样10次
			gAdcPowerValueIndex = 0;
			for(lCnt = 0; lCnt < MAX_POWERADC_TEMPSIZE; lCnt++){
				Total += gAdcTempPowerValue[lCnt];
			}
			vol = Total/MAX_POWERADC_TEMPSIZE;
			gSysPower = (vol/4096.0f)*105.0f;
		}
	}
	gAdcPowerValue = 0xFFFF;
	DrvAdcTriggerConversion(ADCCH1_EXTFUNC_USE);//开始采样	
}

