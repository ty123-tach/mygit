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
//* @{引用头文件声明区域(仅限本文件内使用)
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
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#define REQUEST_BUF_POS	0	/*请求数据缓冲区位置*/
#define REQUEST_DATA_POS 1	/*请求数据位置*/

#define RESPOND_BUF_POS 0	/*响应数据缓冲区位置*/
#define RESPOND_DATA_POS 1	/*响应数据位置*/

#define REQ_BOOTMODE_IN		1	/*请求进入boot模式状态*/
#define REQ_START_UPGRADE	2	/*请求进入开始升级状态*/
#define REQ_RECV_UPGRADE	3	/*请求接收升级数据状态*/
#define REQ_END_UPGRADE		4	/*请求结束升级状态*/
#define REQ_REBOOT_IN		5	/*请求进入重启状态*/
#define REQ_READ_APPINFO	6	/*请求读取APP程序的数据*/
#define REQ_READ_BOOTINFO	7	/*请求读取BOOT程序的数据*/

static s_CFGMAGIC_T gFixCfg;	/*固定区域内容*/
static unsigned int gCurrentUpgradeBlock = 0;
static unsigned int gLastUpgradeBlock = 0xFFFF;
static unsigned int gCurrentReadBlock = 0;
static unsigned int gLastReadBlock = 0xFFFF;
static unsigned char gUpgradeStatus = 0;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void AppDataErase(void); 
extern void ZkMemCpy(void *dst,void *src,const unsigned long len);
//**
// * @}
 
/*!
 * AppUpgradeHandle
 * upgrade app handele
 * 升级APP处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void AppUpgradeHandle(void)
{
	unsigned char lReqBuf[69];
	
	ZkPtlCopyFrameData(TAG04_CMD_REQUEST,&lReqBuf);
		
	if(lReqBuf[REQUEST_BUF_POS] == REQ_BOOTMODE_IN){/*请求进入boot模式状态*/
		gUpgradeStatus = REQ_BOOTMODE_IN;
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_START_UPGRADE){/*请求进入开始升级状态*/
		if(gUpgradeStatus != REQ_START_UPGRADE){
			gUpgradeStatus = REQ_START_UPGRADE;
			//取出新程序的大小以及校验值
			gFixCfg.Magic = 0xFFFFFFFF;
			gFixCfg.AppSize = lReqBuf[REQUEST_DATA_POS]|(lReqBuf[REQUEST_DATA_POS+1]<<8)
								|(lReqBuf[REQUEST_DATA_POS+2]<<16)|(lReqBuf[REQUEST_DATA_POS+3]<<24);
			gFixCfg.CheckSum = lReqBuf[REQUEST_DATA_POS+4]|(lReqBuf[REQUEST_DATA_POS+5]<<8)
								|(lReqBuf[REQUEST_DATA_POS+6]<<16)|(lReqBuf[REQUEST_DATA_POS+7]<<24);
			//擦除旧程序
			AppDataErase();
		}
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_RECV_UPGRADE){/*请求接收升级数据状态*/
		gUpgradeStatus = REQ_RECV_UPGRADE;
		gCurrentUpgradeBlock = (lReqBuf[68]<<24)|(lReqBuf[67]<<16)|(lReqBuf[66]<<8)|(lReqBuf[65]);
		if(gCurrentUpgradeBlock != gLastUpgradeBlock){//新数据包到达
			gLastUpgradeBlock = gCurrentUpgradeBlock;
			if(gCurrentUpgradeBlock < 320){//数据块索引在20K范围内
				DrvFlashBlock64Write(CFG_FLASH_APP_CODEADR+gCurrentUpgradeBlock*64,//写入数据包
				&lReqBuf[REQUEST_DATA_POS]);
			}else{
				lReqBuf[RESPOND_BUF_POS] = 0xFD;
			}
		}else{
			lReqBuf[RESPOND_BUF_POS] = 0xFE;
		}	
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_END_UPGRADE){/*请求结束升级状态*/
		if(gUpgradeStatus != REQ_END_UPGRADE){
			gUpgradeStatus = REQ_END_UPGRADE;
			FixConfigModify(gFixCfg.Magic,gFixCfg.AppSize,gFixCfg.CheckSum);//写入程序信息
		}
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_REBOOT_IN){/*请求进入重启状态*/
		gUpgradeStatus = REQ_REBOOT_IN;
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_READ_APPINFO){/*请求读取APP程序的数据*/
		gCurrentReadBlock = lReqBuf[REQUEST_DATA_POS]|(lReqBuf[REQUEST_DATA_POS+1]<<8)
							|(lReqBuf[REQUEST_DATA_POS+2]<<16)|(lReqBuf[REQUEST_DATA_POS+3]<<24);
		if(gCurrentReadBlock != gLastReadBlock){
			gLastReadBlock = gCurrentReadBlock;
			if(gCurrentReadBlock < 320){
				ZkMemCpy(&lReqBuf[RESPOND_DATA_POS],//读取索引对应的app数据块
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
	}else if(lReqBuf[REQUEST_BUF_POS] == REQ_READ_BOOTINFO){/*请求读取BOOT程序的数据*/	
		gCurrentReadBlock = lReqBuf[REQUEST_DATA_POS]|(lReqBuf[REQUEST_DATA_POS+1]<<8)
							|(lReqBuf[REQUEST_DATA_POS+2]<<16)|(lReqBuf[REQUEST_DATA_POS+3]<<24);
		if(gCurrentReadBlock != gLastReadBlock){
			gLastReadBlock = gCurrentReadBlock;
			if(gCurrentReadBlock < 128){
				ZkMemCpy(&lReqBuf[RESPOND_DATA_POS],//读取索引对应的boot数据块
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
	}else{/*未知指令*/
		lReqBuf[RESPOND_BUF_POS] = 0xFF;
	}
	ZkPtlSetFrameData(TAG05_CMD_RES,lReqBuf);//响应操作结果
	if(gUpgradeStatus == REQ_REBOOT_IN){
		DrvTickDelayMs(50);
		ZkPtlSetFrameData(TAG05_CMD_RES,lReqBuf);//响应操作结果
		__disable_irq();
		NVIC_SystemReset();
		while(1);	
	}
}
/*!
 * AppDataErase
 * erase application data
 * 擦除应用程序数据
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
