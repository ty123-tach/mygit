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
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "PowerMonitor.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
#include "System.h"
#include "DrvTimer.h"
#include "DrvADC.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define MAX_POWERADC_SAMPLE_FREQ	110	/*�����������λms*/
#define MAX_POWERADC_TEMPSIZE		10	/*��������*/
static unsigned char gAdcPowerValueIndex = 0;
static unsigned short gAdcPowerValue = 0xFFFF;//��ѹ����ֵ
static unsigned short gAdcTempPowerValue[MAX_POWERADC_TEMPSIZE];
static unsigned int gAdcPowerTimeStamp = 0;//��ѹ����ʱ���
static float gSysPower;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
 
//**
// * @}
/*!
 * GetSystemPower
 * ��ȡϵͳ��ѹ
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
 * Ĭ�ϻ�ȡADCֵ
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
 * ��Դ��س�ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void PowerMonitorInit(void)
{
    //����оƬ34��������Ϊģ��ADC����
	DrvGpioSetPinMux(PORTA,PORTA_PIN1,PIN34_ADC0_SE1_ACMP0_IN1_TSI0_CH18);//PTA1 
	DrvAdcGetValueFuncRegister(ADCCH1_EXTFUNC_USE,GetPowerValue);
	gAdcPowerValue = 0xFFFF;
	gAdcPowerValueIndex = 0;
	gSysPower = 0;
}
/*!
 * PowerSystemAdcHandle
 * ��ԴϵͳADC����
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
	if(gAdcPowerValue != 0xFFFF){//��������
		gAdcTempPowerValue[gAdcPowerValueIndex++] = gAdcPowerValue;
		if(gAdcPowerValueIndex >= MAX_POWERADC_TEMPSIZE){//����10��
			gAdcPowerValueIndex = 0;
			for(lCnt = 0; lCnt < MAX_POWERADC_TEMPSIZE; lCnt++){
				Total += gAdcTempPowerValue[lCnt];
			}
			vol = Total/MAX_POWERADC_TEMPSIZE;
			gSysPower = (vol/4096.0f)*105.0f;
		}
	}
	gAdcPowerValue = 0xFFFF;
	DrvAdcTriggerConversion(ADCCH1_EXTFUNC_USE);//��ʼ����	
}

