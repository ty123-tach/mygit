/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: DrvTimer.c
	Author: Zhang zhiwei Version: V0 Date: 02. July. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "DrvTimer.h"
#include "System.h"
#include "MKE16Z4.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
/*clock source from SCG*/
#define SCG_CLK_SRC		0
/*clock source from PMC low power oscillator*/
#define LPO_CLK_SRC		1
/*clock source form RTC clock in*/
#define RTC_CLKIN_SRC	2

/*select clock source*/
#define LPTMR0_SELECT_CLOCK	LPO_CLK_SRC

#if (LPTMR0_SELECT_CLOCK == LPO_CLK_SRC)
	#define FREQ_SOURCE_CLK	128000	/*128K*/
	#define FREQ_SOURCE_DIV	1	/*div2*/
#endif
#define BASE_MS_TICK	(FREQ_SOURCE_CLK/FREQ_SOURCE_DIV/1000)
#define SYSTEM_TICK_SPACE	BASE_MS_TICK
static unsigned int gSystemTick = 0;
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)

//**
// * @}
/*!
 * DrvTickDelayMs
 * delay tick
 * �����ʱ
 * @param[in]   ms����ʱʱ��
 * @param[out]  None
 * @retval  	None
 */
void DrvTickDelayMs(unsigned int ms)
{
	unsigned int temp = gSystemTick;
	
	while((gSystemTick - temp) < ms)
		__asm("NOP"); /* delay */
}
/*!
 * DrvGetTimerMsTick
 * ms tick
 * ��ȡϵͳ�δ�ʱ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
unsigned int DrvGetTimerMsTick(void)
{
	return gSystemTick;
}
/*!
 * DrvTimerInit
 * ��ʱ����ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvTimerInit(void)
{
	/* Ungate the LPTMR clock*/
	PeripheralClockEnable(PCC_LPTMR0_INDEX);
	LPTMR0->CSR = 0;
	/*select low power oscillator clock 128K default prescaler clock by 1*/
	LPTMR0->PSR |= LPTMR_PSR_PBYP_MASK|LPTMR_PSR_PCS(LPTMR0_SELECT_CLOCK);
	LPTMR0->CMR = SYSTEM_TICK_SPACE;
	/*enable Timer interrupt*/
	LPTMR0->CSR |= LPTMR_CSR_TIE_MASK;
	/*enable system interrupt*/
    NVIC_SetPriority(PWT_LPTMR0_IRQn,1);
	NVIC_EnableIRQ(PWT_LPTMR0_IRQn);	
	/*enable timer*/
	LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
	gSystemTick = 0;
}
/*!
 * DrvTimerDeinit
 * ��ʱ����λ��ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvTimerDeinit(void)
{
	NVIC_DisableIRQ(PWT_LPTMR0_IRQn);
	/* Disable the LPTMR and reset the internal logic */
	LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
	/* Gate the LPTMR clock*/
	PCC->CLKCFG[PCC_LPTMR0_INDEX] &= ~PCC_CLKCFG_CGC_MASK;
	/* Gate OFF External clock*/
	PCC->CLKCFG[PCC_LPTMR0_INDEX] &= ~PCC_CLKCFG_PCS(0);
}
/*!
 * PWT_LPTMR0_IRQHandler
 * ��ʱ���ж�
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void PWT_LPTMR0_IRQHandler(void)
{
	/*clear timer compare flag*/
	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
	gSystemTick++;
}
