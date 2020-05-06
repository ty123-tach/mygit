/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvPwm.c
	Author: ZhangZhiwei Version: V0 Date: 2019-08-13
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "DrvPwm.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
#include "System.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
 
//**
// * @}
 
/*!
 * DrvPwmInt
 * PWM��ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvPwmInt(void)
{
    //оƬ����2��������ΪPWM����
	DrvGpioSetPinMux(PORTD,PORTD_PIN0,PIN2_FTM0_CH2);//PTD0   FTM0_CH2   PWM 
	DrvGpioSetPinMux(PORTD,PORTD_PIN1,PIN1_FTM0_CH3);//PTD1   FTM0_CH3   PWM 
	 /* Ungate the FTM clock*/
	PeripheralClockEnable(PCC_FLEXTMR0_INDEX);
	
	FTM0->MODE = FTM_MODE_WPDIS_MASK;
	/*set polarity*/
//	FTM0->POL |= (1<<2)|(1<<3);//CH2&CH3 channel polarity is active low
	FTM0->SYNC &= ~FTM_SYNC_SYNCHOM_MASK;
	FTM0->SC |= FTM_SC_PS(3);//48Mhz divide by 8
	FTM0->SC |= FTM_SC_CPWMS_MASK;
	FTM0->CONF = 0;
	FTM0->MOD = 3000;
	FTM0->CNTIN = 0;
	FTM0->CONTROLS[2].CnV = 0;
	FTM0->CONTROLS[2].CnSC = 0x08;
	FTM0->CONTROLS[3].CnV = 0;
	FTM0->CONTROLS[3].CnSC = 0x08;	
	FTM0->SC |= FTM_SC_PWMEN2(1)|FTM_SC_PWMEN3(1);
	FTM0->PWMLOAD |= FTM_PWMLOAD_CH2SEL(1)|FTM_PWMLOAD_CH3SEL(1);
	FTM0->CNT = 0;
	FTM0->SC |= FTM_SC_CLKS(1);
}
/*!
 * DrvSetPwmValue
 * ����PWMֵ
 * @param[in]   ch��ͨ��	
				value��ռ�ձ�
 * @param[out]  None
 * @retval  	 None
 */
void DrvSetPwmValue(unsigned char ch,unsigned int value)
{
	FTM0->CONTROLS[ch].CnV = value;
}
