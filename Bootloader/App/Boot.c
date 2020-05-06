/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: boot.c
	Author: Zhang zhiwei Version: V0 Date: 24. June. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "Boot.h"
#include "System.h"
#include "DrvTimer.h"
#include "DrvUart.h"
#include "DrvFlash.h"
#include "ZkPtl.h"
#include "Configure.h"
#include "SoftTagInfo.h"
#include "AppConfig.h"
#include "AppUpgrade.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
static unsigned char gProgramError = 0;
static unsigned int gReportTimeStamp = 0;
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
unsigned int ZkChkVrifi(unsigned char *Adr,unsigned long len);
void BootInit(void);
extern void ZkMemCpy(void *dst,void *src,const unsigned long len);
unsigned char FactoryInitializationCheck(unsigned int size,unsigned int crc);
void BootReportProgramError(void);
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
	DrvTimerInit();
	DrvUartInit();
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
	ZkPtlInit();
	BootInit();
	__enable_irq();
}
/*!
 * BootHandle
 * Handle boot
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BootHandle(void)
{
	e_TAGTYPE_T lTag;
	
	BootReportProgramError();
	lTag = ZkPtlMonitor();
	if(lTag == TAG_UNKNOW)
		return;
	gProgramError = 0;
	if(lTag < TAG02_CONFIG_OPT){//��ǩ��Ϣ��ѯ��Χ��
		SoftTagInfoHandle();
	}else if(lTag < TAG04_CMD_REQUEST){//������Ϣ��Χ�ڵı�ǩ
		AppConfigHandle();
	}else{//���������Χ�ڵı�ǩ
		AppUpgradeHandle();
	}
}
/*!
 * BootReportProgramError
 * ��������쳣
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BootReportProgramError(void)
{
	s_BWTAG05_T report;
	if(gProgramError == 0)
		return;
	if((DrvGetTimerMsTick() - gReportTimeStamp) < 3000)
		return;
	gReportTimeStamp = DrvGetTimerMsTick();
	report.Respond = CMD_BOOTMODE_SUCCESS;
	ZkPtlSetFrameData(TAG05_CMD_RES,&report);
}
/*!
 * BootInit
 * ������ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BootInit(void)
{
	s_CFGMAGIC_T lCfg;
	
	ZkMemCpy(&lCfg,(unsigned char*)CFG_FLASH_APP_CFGADR,sizeof(s_CFGMAGIC_T));
	gProgramError = 0;
	if(FactoryInitializationCheck(lCfg.AppSize,lCfg.CheckSum)){//�����쳣������bootģʽ
		gProgramError = 1;
		return;
	}
	if(lCfg.Magic == CFG_MAGIC_VALUE){//ħ��ֵ����,����bootģʽ
		FixConfigModify(0xFFFFFFFF,lCfg.AppSize,lCfg.CheckSum);
		return;
	}
	JumpToApplication();
}
/*!
 * FactoryInitializationCheck
 * factory initialization state check
 * @param[in]   None
 * @param[out]  None
 * @retval  	0: not initialization 1: initialization state
 */
unsigned char FactoryInitializationCheck(unsigned int size,unsigned int crc)
{
	unsigned int Res;
	
	if((size > CFG_FLASH_APPCODE_SIZE)||(crc == 0xFFFFFFFF)){
		return 1;
	}else{
		Res = ZkChkVrifi((unsigned char*)CFG_FLASH_APP_CODEADR,size);
		if(crc != Res)
			return 1;
	} 
	return 0;
}
/*!
 * ZkChkVrifi
 * ����У���㷨
 * @param[in]   Adr�� ���ݵ�ַ
				len:	���ݳ���
 * @param[out]  None
 * @retval  	lMnkValue:У����
 */
unsigned int ZkChkVrifi(unsigned char *Adr,unsigned long len) 
{ 
	unsigned int lMnkValue = 0x1717;  
	int lCnt; 
	while(len--) 
	{ 
		lMnkValue ^= *Adr++;    
		for(lCnt = 0; lCnt < 8; lCnt++) 
		{
			if((lMnkValue & 0x0001) == 0) 
			{
				lMnkValue =( lMnkValue >> 1 )^0XA001; 
			}else{
				lMnkValue = lMnkValue >> 1; 
			}
		}
	} 
	return lMnkValue; 
}

