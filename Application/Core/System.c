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
//* @{引用头文件声明区域(仅限本文件内使用)
#include "System.h"
#include "MKE16Z4.h"
#include "DrvTimer.h"
#include "DrvUart.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏) 
/* Default System clock value */
#define DEFAULT_SYSTEM_CLOCK           48000000u           

static unsigned int gSystemCoreClock = DEFAULT_SYSTEM_CLOCK;	/*默认系统时钟频率*/

//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)

//**
// * @}
/*!
 * PeripheralClockSrcSelect
 * 外设时钟源选择
 * @param[in]   perinum：外设编号
				clksrc：时钟源
 * @param[out]  None
 * @retval  	None
 */
void PeripheralClockSrcSelect(unsigned int perinum,unsigned char clksrc)
{
	unsigned int reg = PCC->CLKCFG[perinum];

	reg = (reg&(~PCC_CLKCFG_PCS_MASK))|PCC_CLKCFG_PCS(clksrc);
	/*如果外设时钟已使能，先关闭再使能*/
	PCC->CLKCFG[perinum] = reg&(~PCC_CLKCFG_CGC_MASK);
	PCC->CLKCFG[perinum] = reg|PCC_CLKCFG_CGC_MASK;
}
/*!
 * PeripheralClockEnable
 * 使能外设时钟
 * @param[in]   perinum：外设编号
 * @param[out]  None
 * @retval  	None
 */
void PeripheralClockEnable(unsigned int perinum)
{
	PCC->CLKCFG[perinum] |= PCC_CLKCFG_CGC_MASK;
}
/*!
 * getSystemCoreClock
 * 获取系统内核时钟频率
 * @param[in]   None
 * @param[out]  None
 * @retval  	时钟频率
 */
unsigned int getSystemCoreClock(void)
{
	return gSystemCoreClock;
}
/*!
 * getBusClock
 * 获取总线时钟频率
 * @param[in]   None
 * @param[out]  None
 * @retval  	时钟频率
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
 * 获取异步外设时钟频率
 * @param[in]   None
 * @param[out]  None
 * @retval  	时钟频率
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

