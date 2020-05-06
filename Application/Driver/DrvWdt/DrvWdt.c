/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvWdt.c
	Author: ZhangZhiwei Version: V0 Date: 2019-08-14
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "DrvWdt.h"
#include "MKE16Z4.h"
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
 * DrvWdtInit
 * ���Ź���ʼ��
 * @param[in]   en��0-����	1-ʹ��
 * @param[out]  None
 * @retval  	None
 */
void DrvWdtInit(unsigned char en)
{
	__disable_irq();
	WDOG->CNT = WDOG_UPDATE_KEY;//unlock watchdog
	while((WDOG->CS&WDOG_CS_ULK_MASK) == 0);//wait until registers are unlocked
	WDOG->TOVAL = 0xFFFF;//set 2S timeout LPO_CLK = 128K (1/128 * 256 = 2)
	WDOG->CS = WDOG_CS_EN(en)|WDOG_CS_CLK(1)|WDOG_CS_INT(0)|
			   WDOG_CS_WIN(0)|WDOG_CS_UPDATE(1)|WDOG_CS_CMD32EN(1);
	while((WDOG->CS&WDOG_CS_RCS_MASK) == 0);//wait until new configuration takes effect
	__enable_irq();
}
/*!
 * DrvWdtFeed
 * ���Ź�ι��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvWdtFeed(void)
{
//	__disable_irq();
	WDOG->CNT = WDOG_REFRESH_KEY;
//	__enable_irq();
}
