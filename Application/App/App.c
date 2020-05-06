/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: App.c
	Author: Zhang zhiwei Version: V0 Date: 24. June. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "App.h"
#include "DrvTimer.h"
#include "DrvFlash.h"
#include "Configure.h"
#include "DrvUart.h"
#include "ZkPtl.h"
#include "MKE16Z4.h"
#include "DrvADC.h"
#include "PowerMonitor.h"
#include "DebugMsg.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define MAX_APPRUNTIME_TIME	1000	//����ʱ�� ��λms
static unsigned int gAppRunTimeStamp = 0;
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
//**
// * @}
/*!
 * HardwareInit
 * initialization the hardware
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HardwareInit(void)
{
	__disable_irq();
	DrvTimerInit();
	DrvFlashInit();
#if DEBUG_INFO_OUT	
	DebugRegister(DrvUart1SenCh,DrvUartInit);
#else
	DrvUartInit();
#endif
	ZkPtlInit();
	DrvAdcInit();
}
/*!
 * SoftwareInit
 * initialization the software
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SoftwareInit(void)
{
	PowerMonitorInit();
	gAppRunTimeStamp = DrvGetTimerMsTick();
}
/*!
 * AppHandle
 * Ӧ�ô���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void AppHandle(void)
{
	PowerSystemAdcHandle();
}
/*!
 * ZkLiftHandle
 * ����Э�鴦��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ZkLiftHandle(void)
{
	if(DrvGetTimerMsTick() - gAppRunTimeStamp < MAX_APPRUNTIME_TIME)
		return;
	gAppRunTimeStamp = DrvGetTimerMsTick();
	DebugMsg("Now is running lift handle,system power:%0.1fV\r\n",GetSystemPower());
}
/*!
 * ZkPtlHandle
 * �ǿ�Э�鴦��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlHandle(void)
{
	s_BWTAG04_T lRequest;
	e_TAGTYPE_T lTag;
	
	lTag = ZkPtlMonitor();
	if(lTag != TAG04_CMD_REQUEST)//����������ָ��
		return;	
	ZkPtlCopyFrameData(lTag,&lRequest);
	if(lRequest.Request != CMD_BOOTMODE_IN)//�ǽ���BOOTģʽ
		return;
	ZkPtlSetFrameData(TAG05_CMD_RES,&lRequest);
	DrvFlashWriteLongWord(CFG_FLASH_APP_CFGADR,CFG_MAGIC_VALUE);//����boot����ģʽ
	DrvTickDelayMs(500);
	__disable_irq();
	NVIC_SystemReset();
	while(1);		
}


