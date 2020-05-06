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
//* @{引用头文件声明区域(仅限本文件内使用)
#include "AppConfig.h"
#include "Configure.h"
#include "ZkPtl.h"
#include "DrvFlash.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#define BUF_BLOCKINDEX_POS	0

#define BUF_BLOCKCMD_POS	1
#define BUF_CMDDATA_POS		2

#define BUF_BLOCKCMDSTA_POS	1
#define BUF_BLOCKDATA_POS	2

#define CMD_UPDATE_CONFIGINFO	1	/*更新配置信息*/
#define CMD_READ_CONFIGINFO		2	/*读取配置信息*/
#define CMD_ERASE_CONFIGINFO	3	/*删除4K配置信息*/

static unsigned int gCfgSector[256];
static s_CFGMAGIC_T gFixCfg;
static unsigned char gFixCfgRestore = 0;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
extern void ZkMemCpy(void *dst,void *src,const unsigned long len);
//**
// * @}
/*!
 * AppConfigHandle
 * handle the app configure
 * 处理APP程序配置
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void AppConfigHandle(void)
{
	unsigned char lCfgBuf[66];
	
	ZkPtlCopyFrameData(TAG02_CONFIG_OPT,&lCfgBuf);

	if(lCfgBuf[BUF_BLOCKCMD_POS] == CMD_UPDATE_CONFIGINFO){/*更新配置信息*/
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
	}else if(lCfgBuf[BUF_BLOCKCMD_POS] == CMD_READ_CONFIGINFO){/*读取配置信息*/
		if(lCfgBuf[BUF_BLOCKINDEX_POS] < 64){
			ZkMemCpy(&lCfgBuf[BUF_BLOCKDATA_POS],
			(unsigned char*)(CFG_FLASH_APP_CFGADR+lCfgBuf[BUF_BLOCKINDEX_POS]*64),
			64);
		}else{
			lCfgBuf[BUF_BLOCKCMDSTA_POS] = 0xFE;
		}
	}else if(lCfgBuf[BUF_BLOCKCMD_POS] == CMD_ERASE_CONFIGINFO){/*删除4K配置信息*/
		ZkMemCpy(&gFixCfg,(unsigned char*)CFG_FLASH_APP_CFGADR,sizeof(s_CFGMAGIC_T));
		gFixCfgRestore = 1;
		DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART);
		DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART+1);
		DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART+2);
		DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART+3);
	}else{
		lCfgBuf[BUF_BLOCKCMDSTA_POS] = 0xFF;
	}
	ZkPtlSetFrameData(TAG03_CONFIG_RES,lCfgBuf);//响应操作结果
}
/*!
 * FixConfigModify
 * 修改固定配置内容
 * @param[in]   magic：魔术值
				appsize：APP大小
				checksum：校验值
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
