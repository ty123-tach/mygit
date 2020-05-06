/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: AppConfig.c
	Author: ZhangZhiwei Version: V0 Date: 2019-07-30
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "AppConfig.h"
#include "Configure.h"
#include "ZkPtl.h"
#include "DrvFlash.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define BUF_BLOCKINDEX_POS	0

#define BUF_BLOCKCMD_POS	1
#define BUF_CMDDATA_POS		2

#define BUF_BLOCKCMDSTA_POS	1
#define BUF_BLOCKDATA_POS	2

#define CMD_UPDATE_CONFIGINFO	1	/*����������Ϣ*/
#define CMD_READ_CONFIGINFO		2	/*��ȡ������Ϣ*/
#define CMD_ERASE_CONFIGINFO	3	/*ɾ��4K������Ϣ*/

static unsigned int gCfgSector[256];
static s_CFGMAGIC_T gFixCfg;
static unsigned char gFixCfgRestore = 0;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
extern void ZkMemCpy(void *dst,void *src,const unsigned long len);
//**
// * @}
/*!
 * AppConfigHandle
 * handle the app configure
 * ����APP��������
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void AppConfigHandle(void)
{
	unsigned char lCfgBuf[66];
	
	ZkPtlCopyFrameData(TAG02_CONFIG_OPT,&lCfgBuf);

	if(lCfgBuf[BUF_BLOCKCMD_POS] == CMD_UPDATE_CONFIGINFO){/*����������Ϣ*/
		if(lCfgBuf[BUF_BLOCKINDEX_POS] < 64){
			if(lCfgBuf[BUF_BLOCKINDEX_POS] == 0){
				if(gFixCfgRestore){
					ZkMemCpy(&lCfgBuf[BUF_CMDDATA_POS],&gFixCfg,sizeof(s_CFGMAGIC_T));
				}
			}
			DrvFlashBlock64Write((CFG_FLASH_APP_CFGADR+lCfgBuf[BUF_BLOCKINDEX_POS]*64),
						&lCfgBuf[BUF_CMDDATA_POS]);
		}else{
			lCfgBuf[BUF_BLOCKCMDSTA_POS] = 0xFD;
		}
	}else if(lCfgBuf[BUF_BLOCKCMD_POS] == CMD_READ_CONFIGINFO){/*��ȡ������Ϣ*/
		if(lCfgBuf[BUF_BLOCKINDEX_POS] < 64){
			ZkMemCpy(&lCfgBuf[BUF_BLOCKDATA_POS],
			(unsigned char*)(CFG_FLASH_APP_CFGADR+lCfgBuf[BUF_BLOCKINDEX_POS]*64),
			64);
		}else{
			lCfgBuf[BUF_BLOCKCMDSTA_POS] = 0xFE;
		}
	}else if(lCfgBuf[BUF_BLOCKCMD_POS] == CMD_ERASE_CONFIGINFO){/*ɾ��4K������Ϣ*/
		ZkMemCpy(&gFixCfg,(unsigned char*)CFG_FLASH_APP_CFGADR,sizeof(s_CFGMAGIC_T));
		gFixCfgRestore = 1;
		DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART);
		DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART+1);
		DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART+2);
		DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART+3);
	}else{
		lCfgBuf[BUF_BLOCKCMDSTA_POS] = 0xFF;
	}
	ZkPtlSetFrameData(TAG03_CONFIG_RES,lCfgBuf);//��Ӧ�������
}
/*!
 * FixConfigModify
 * �޸Ĺ̶���������
 * @param[in]   magic��ħ��ֵ
				appsize��APP��С
				checksum��У��ֵ
 * @param[out]  None
 * @retval  	None
 */
void FixConfigModify(unsigned int magic,unsigned int appsize,unsigned int checksum)
{
	unsigned int lCnt;
	
	ZkMemCpy(gCfgSector,(unsigned char*)CFG_FLASH_APP_CFGADR,CFG_FLASH_SECTORSIZE);
	DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART);
	gCfgSector[0] = magic;
	gCfgSector[1] = checksum;
	gCfgSector[2] = appsize;
	for(lCnt = 0; lCnt < 256; lCnt++){
		DrvFlashWriteLongWord(CFG_FLASH_APP_CFGADR+4*lCnt,gCfgSector[lCnt]);
	}
}
