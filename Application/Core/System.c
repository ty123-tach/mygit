/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: System.c
	Author: Zhang zhiwei Version: V0 Date: 27. June. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "System.h"
#include "MKE16Z4.h"
#include "DrvTimer.h"
#include "DrvUart.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�) 
/* Default System clock value */
#define DEFAULT_SYSTEM_CLOCK           48000000u           

static unsigned int gSystemCoreClock = DEFAULT_SYSTEM_CLOCK;	/*Ĭ��ϵͳʱ��Ƶ��*/

//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)

//**
// * @}
/*!
 * PeripheralClockSrcSelect
 * ����ʱ��Դѡ��
 * @param[in]   perinum��������
				clksrc��ʱ��Դ
 * @param[out]  None
 * @retval  	None
 */
void PeripheralClockSrcSelect(unsigned int perinum,unsigned char clksrc)
{
	unsigned int reg = PCC->CLKCFG[perinum];

	reg = (reg&(~PCC_CLKCFG_PCS_MASK))|PCC_CLKCFG_PCS(clksrc);
	/*�������ʱ����ʹ�ܣ��ȹر���ʹ��*/
	PCC->CLKCFG[perinum] = reg&(~PCC_CLKCFG_CGC_MASK);
	PCC->CLKCFG[perinum] = reg|PCC_CLKCFG_CGC_MASK;
}
/*!
 * PeripheralClockEnable
 * ʹ������ʱ��
 * @param[in]   perinum��������
 * @param[out]  None
 * @retval  	None
 */
void PeripheralClockEnable(unsigned int perinum)
{
	PCC->CLKCFG[perinum] |= PCC_CLKCFG_CGC_MASK;
}
/*!
 * getSystemCoreClock
 * ��ȡϵͳ�ں�ʱ��Ƶ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	ʱ��Ƶ��
 */
unsigned int getSystemCoreClock(void)
{
	return gSystemCoreClock;
}
/*!
 * getBusClock
 * ��ȡ����ʱ��Ƶ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	ʱ��Ƶ��
 */
unsigned int getBusClock(void)
{
	unsigned char Div;
	
	Div = SCG->RCCR&SCG_RCCR_DIVSLOW_MASK;
	if((Div == 0)||(Div > 7))
		return 0;
	return 	gSystemCoreClock/(Div+1);
}
/*!
 * getAsyncClock
 * ��ȡ�첽����ʱ��Ƶ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	ʱ��Ƶ��
 */
unsigned int getAsyncClock(void)
{
	unsigned short int Divider;
	unsigned char Div[] = {1,2,4,8,16,32,64};
	
	Divider = (SCG->FIRCDIV&SCG_FIRCDIV_FIRCDIV2_MASK)>>SCG_FIRCDIV_FIRCDIV2_SHIFT;
	
	if(Divider == 0)
		return 0;
	return gSystemCoreClock/Div[Divider-1];
}

