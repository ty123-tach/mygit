/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: AppUpgrade.c
	Author: ZhangZhiwei Version: V0 Date: 2019-07-30
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "AppUpgrade.h"
#include "Configure.h"
#include "ZkPtl.h"
#include "DrvFlash.h"
#include "AppConfig.h"
#include "MKE16Z4.h"
#include "DrvTimer.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define REQUEST_BUF_POS	0	/*�������ݻ�����λ��*/
#define REQUEST_DATA_POS 1	/*��������λ��*/

#define RESPOND_BUF_POS 0	/*��Ӧ���ݻ�����λ��*/
#define RESPOND_DATA_POS 1	/*��Ӧ����λ��*/

#define REQ_BOOTMODE_IN		1	/*�������bootģʽ״̬*/
#define REQ_START_UPGRADE	2	/*������뿪ʼ����״̬*/
#define REQ_RECV_UPGRADE	3	/*���������������״̬*/
#define REQ_END_UPGRADE		4	/*�����������״̬*/
#define REQ_REBOOT_IN		5	/*�����������״̬*/
#define REQ_READ_APPINFO	6	/*�����ȡAPP���������*/
#define REQ_READ_BOOTINFO	7	/*�����ȡBOOT���������*/

static s_CFGMAGIC_T gFixCfg;	/*�̶���������*/
static unsigned int gCurrentUpgradeBlock = 0;
static unsigned int gLastUpgradeBlock = 0xFFFF;
static unsigned int gCurrentReadBlock = 0;
static unsigned int gLastReadBlock = 0xFFFF;
static unsigned char gUpgradeStatus = 0;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void AppDataErase(void); 
extern void ZkMemCpy(void *dst,void *src,const unsigned long len);
//**
// * @}
 
/*!
 * AppUpgradeHandle
 * upgrade app handele
 * ����APP����
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void AppUpgradeHandle(void)
{
	unsigned char lReqBuf[69];
	
	ZkPtlCopyFrameData(TAG04_CMD_REQUEST,&lReqBuf);
		
	if(lReqBuf[REQUEST_BUF_POS] == REQ_BOOTMODE_IN){/*�������bootģʽ״̬*/
		gUpgradeStatus = REQ_BOOTMODE_IN;
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_START_UPGRADE){/*������뿪ʼ����״̬*/
		if(gUpgradeStatus != REQ_START_UPGRADE){
			gUpgradeStatus = REQ_START_UPGRADE;
			//ȡ���³���Ĵ�С�Լ�У��ֵ
			gFixCfg.Magic = 0xFFFFFFFF;
			gFixCfg.AppSize = lReqBuf[REQUEST_DATA_POS]|(lReqBuf[REQUEST_DATA_POS+1]<<8)
								|(lReqBuf[REQUEST_DATA_POS+2]<<16)|(lReqBuf[REQUEST_DATA_POS+3]<<24);
			gFixCfg.CheckSum = lReqBuf[REQUEST_DATA_POS+4]|(lReqBuf[REQUEST_DATA_POS+5]<<8)
								|(lReqBuf[REQUEST_DATA_POS+6]<<16)|(lReqBuf[REQUEST_DATA_POS+7]<<24);
			//�����ɳ���
			AppDataErase();
		}
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_RECV_UPGRADE){/*���������������״̬*/
		gUpgradeStatus = REQ_RECV_UPGRADE;
		gCurrentUpgradeBlock = (lReqBuf[68]<<24)|(lReqBuf[67]<<16)|(lReqBuf[66]<<8)|(lReqBuf[65]);
		if(gCurrentUpgradeBlock != gLastUpgradeBlock){//�����ݰ�����
			gLastUpgradeBlock = gCurrentUpgradeBlock;
			if(gCurrentUpgradeBlock < 320){//���ݿ�������20K��Χ��
				DrvFlashBlock64Write(CFG_FLASH_APP_CODEADR+gCurrentUpgradeBlock*64,//д�����ݰ�
				&lReqBuf[REQUEST_DATA_POS]);
			}else{
				lReqBuf[RESPOND_BUF_POS] = 0xFD;
			}
		}else{
			lReqBuf[RESPOND_BUF_POS] = 0xFE;
		}	
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_END_UPGRADE){/*�����������״̬*/
		if(gUpgradeStatus != REQ_END_UPGRADE){
			gUpgradeStatus = REQ_END_UPGRADE;
			FixConfigModify(gFixCfg.Magic,gFixCfg.AppSize,gFixCfg.CheckSum);//д�������Ϣ
		}
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_REBOOT_IN){/*�����������״̬*/
		gUpgradeStatus = REQ_REBOOT_IN;
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_READ_APPINFO){/*�����ȡAPP���������*/
		gCurrentReadBlock = lReqBuf[REQUEST_DATA_POS]|(lReqBuf[REQUEST_DATA_POS+1]<<8)
							|(lReqBuf[REQUEST_DATA_POS+2]<<16)|(lReqBuf[REQUEST_DATA_POS+3]<<24);
		if(gCurrentReadBlock != gLastReadBlock){
			gLastReadBlock = gCurrentReadBlock;
			if(gCurrentReadBlock < 320){
				ZkMemCpy(&lReqBuf[RESPOND_DATA_POS],//��ȡ������Ӧ��app���ݿ�
					(unsigned char*)(CFG_FLASH_APP_CODEADR+gCurrentReadBlock*64),
					64);
				lReqBuf[65] = gCurrentReadBlock&0xFF;
				lReqBuf[66] = (gCurrentReadBlock>>8)&0xFF;
				lReqBuf[67] = (gCurrentReadBlock>>16)&0xFF;
				lReqBuf[68] = (gCurrentReadBlock>>24)&0xFF;
			}else{
				lReqBuf[RESPOND_BUF_POS] = 0xFB;
			}
		}else{
			lReqBuf[RESPOND_BUF_POS] = 0xFC;
		}
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_READ_BOOTINFO){/*�����ȡBOOT���������*/	
		gCurrentReadBlock = lReqBuf[REQUEST_DATA_POS]|(lReqBuf[REQUEST_DATA_POS+1]<<8)
							|(lReqBuf[REQUEST_DATA_POS+2]<<16)|(lReqBuf[REQUEST_DATA_POS+3]<<24);
		if(gCurrentReadBlock != gLastReadBlock){
			gLastReadBlock = gCurrentReadBlock;
			if(gCurrentReadBlock < 128){
				ZkMemCpy(&lReqBuf[RESPOND_DATA_POS],//��ȡ������Ӧ��boot���ݿ�
					(unsigned char*)(CFG_FLASH_BOOT_CODEADR+gCurrentReadBlock*64),
					64);
				lReqBuf[65] = gCurrentReadBlock&0xFF;
				lReqBuf[66] = (gCurrentReadBlock>>8)&0xFF;
				lReqBuf[67] = (gCurrentReadBlock>>16)&0xFF;
				lReqBuf[68] = (gCurrentReadBlock>>24)&0xFF;					
			}else{
				lReqBuf[RESPOND_BUF_POS] = 0xF9;
			}
		}else{
			lReqBuf[RESPOND_BUF_POS] = 0xFA;
		}		
	}else{/*δָ֪��*/
		lReqBuf[RESPOND_BUF_POS] = 0xFF;
	}
	ZkPtlSetFrameData(TAG05_CMD_RES,lReqBuf);//��Ӧ�������
	if(gUpgradeStatus == REQ_REBOOT_IN){
		DrvTickDelayMs(50);
		ZkPtlSetFrameData(TAG05_CMD_RES,lReqBuf);//��Ӧ�������
		__disable_irq();
		NVIC_SystemReset();
		while(1);	
	}
}
/*!
 * AppDataErase
 * erase application data
 * ����Ӧ�ó�������
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void AppDataErase(void)
{
	unsigned char lCnt;
	
	for(lCnt = CFG_FLASH_APPCODE_INDEXSTART; lCnt < CFG_FLASH_TOTAL_SECTORNUM; lCnt++){
		DrvFlashEraseSector(lCnt);
	}
}
