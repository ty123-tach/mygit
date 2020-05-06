/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: LeagueLopPtl.c
	Author: ZhiweiZhang Version: V0 Date: 2019-12-29
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "LeagueLopPtl.h"
#include "SegLedLopCtrl.h"
#include "DrvCan.h"
#include "DrvTimer.h"
#include "DebugMsg.h"
#include "Configure.h"
#include "DrvFlash.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)

#define LOP_WORK_CFG_ADR	0x00002400	//LOP配置信息地址

#define CAN_LOPDISP_ID			0x01		//LOP显示帧ID
#define CAN_FDUPTBN_LIGHT_ID	0x525064	//LOP前门上按钮灯ID
#define CAN_FDDNBTN_LIGHT_ID	0x525065	//LOP前门下按钮灯ID
#define CAN_BDUPTBN_LIGHT_ID	0x525066	//LOP后门上按钮灯ID
#define CAN_BDDNBTN_LIGHT_ID	0x525067	//LOP后门下按钮灯ID
#define CAN_MASTER_SCAN_ID		0x525068	//控制柜扫描ID
#define CAN_BASE_FDBTN_ID		0x5250C8	//前门外呼按钮ID基地址，应答ID=0x5250C8+所在楼层
#define CAN_BASE_BDBTN_ID		0x525109	//后门外呼按钮ID基地址，应答ID=0x525109+所在楼层
#define CAN_SCAN_RESPOND_ID		0x52514A	//应答控制柜扫描ID


/*COP CAN配置信息*/
static s_USRDATA_T gLopCanCfg; 
static s_CANFRAME_T gRecvFrame;
static s_CANFRAME_T gDispFrame;		//显示帧数据
static s_CANFRAME_T gFdUpBtnFrame;	//前门上按钮帧数据
static s_CANFRAME_T gFdDnBtnFrame;	//前门下按钮帧数据
static s_CANFRAME_T gBdUpBtnFrame;	//后门上按钮帧数据
static s_CANFRAME_T gBdDnBtnFrame;	//后门下按钮帧数据
static s_CANFRAME_T gScanFrame;		//控制柜扫描外呼板帧数据

#define FRAME_DISP_SHIFT	(0x01U)
#define FRAME_DISP_MASK		(0xFEU)

#define FRAME_FDUPBTN_SHIFT	(0x02U)
#define FRAME_FDUPBTN_MASK	(0xFDU)

#define FRAME_FDDNBTN_SHIFT	(0x04U)
#define FRAME_FDDNBTN_MASK	(0xFBU)

#define FRAME_BDUPBTN_SHIFT	(0x08U)
#define FRAME_BDUPBTN_MASK	(0xF7U)

#define FRAME_BDDNBTN_SHIFT	(0x10U)
#define FRAME_BDDNBTN_MASK	(0xEFU)

#define FRAME_SCAN_SHIFT	(0x20U)
#define FRAME_SCAN_MASK		(0xDFU)

static unsigned char gRecvFrameFlag;
static unsigned char gLopType;		//LOP类型 0-前门 1-后门
static unsigned char gLopFloorAdr;	//LOP地址 范围1-64
static unsigned char gLopButtonSta;	//按钮状态
static unsigned char gLopButtonLight;//按钮灯状态
static unsigned char gLopButtonChange;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void LeagueLopCanIrqHandle(void);
void DisplayFrameHandle(void);
void FrontDoorUpBtnLightHandle(void);
void FrontDoorDnBtnLightHandle(void);
void BackDoorUpBtnLightHandle(void);
void BackDoorDnBtnLightHandle(void);
void ScanFrameHandle(void);
void LeagueLopReportButtonStatus(void);
unsigned char HexToAscii(unsigned char value);
//**
// * @}
/*!
 * getLeagueLopWorkMode
 * 获取LOP工作模式
 * @param[in]   None
 * @param[out]  None
 * @retval  	1：前门	2：后门
 */
unsigned char getLeagueLopWorkMode(void)
{
	return gLopType;
}
/*!
 * getLeagueLopFloorAdr
 * 获取LOP楼层地址
 * @param[in]   None
 * @param[out]  None
 * @retval  	楼层地址
 */
unsigned char getLeagueLopFloorAdr(void)
{
	return gLopFloorAdr;
}
/*!
 * getLeagueLopButtonLight
 * 获取按钮灯状态
 * @param[in]   None
 * @param[out]  None
 * @retval  	灯状态
 */
unsigned char getLeagueLopButtonLight(void)
{
	return gLopButtonLight;
}
/*!
 * setLeagueLopButton
 * 设置LOP按钮
 * @param[in]   btn：按钮状态
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopButton(unsigned char btn)
{
	if(btn == gLopButtonSta)
		return;
	gLopButtonSta = btn;
	gLopButtonChange = 1;
}
/*!
 * setLeagueLopFireBtn
 * 设置消防按钮状态
 * @param[in]   sta：非0：按下	0：松开
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopFireBtn(unsigned char sta)
{
	if(sta){
		if((gLopButtonSta&0x04) == 0){
			gLopButtonSta |= 0x04;
			gLopButtonChange = 1;
		}
	}else{
		if(gLopButtonSta&0x04){
			gLopButtonSta &= 0xFB;
			gLopButtonChange = 1;
		}
	}
}
/*!
 * setLeagueLopLockBtn
 * 设置锁梯按钮状态
 * @param[in]   sta：非0：按下	0：松开
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopLockBtn(unsigned char sta)
{
	if(sta){
		if((gLopButtonSta&0x08) == 0){
			gLopButtonSta |= 0x08;
			gLopButtonChange = 1;
		}
	}else{
		if(gLopButtonSta&0x08){
			gLopButtonSta &= 0xF7;
			gLopButtonChange = 1;
		}
	}
}
/*!
 * setLeagueLopUpBtn
 * 设置上行按钮状态
 * @param[in]   sta：非0：按下	0：松开
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopUpBtn(unsigned char sta)
{
	if(sta){
		if((gLopButtonSta&0x01) == 0){
			gLopButtonSta |= 0x01;
			gLopButtonChange = 1;
		}
	}else{
		if(gLopButtonSta&0x01){
			gLopButtonSta &= 0xFE;
			gLopButtonChange = 1;
		}
	}
}
/*!
 * setLeagueLopUpBtn
 * 设置下行按钮状态
 * @param[in]   sta：非0：按下	0：松开
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopDnBtn(unsigned char sta)
{
	if(sta){
		if((gLopButtonSta&0x02) == 0){
			gLopButtonSta |= 0x02;
			gLopButtonChange = 1;
		}
	}else{
		if(gLopButtonSta&0x02){
			gLopButtonSta &= 0xFD;
			gLopButtonChange = 1;
		}
	}
}
/*!
 * setLeagueLopAdr
 * 设置LOP地址
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopAdr(unsigned char type,unsigned char adr)
{
	unsigned int lLopCfg;
	if((type == 0)||(type > 2)||(adr == 0)||(adr > 64))
		return;
	if((gLopType == type)&&(adr == gLopFloorAdr))
		return;
	gLopType = type;
	gLopFloorAdr = adr;
	lLopCfg = gLopFloorAdr;
	lLopCfg <<= 8;
	lLopCfg |= gLopType;
	DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART+1);
	DrvFlashWriteLongWord(LOP_WORK_CFG_ADR,lLopCfg);
}
/*!
 * LeagueLopPtlInit
 * 联盟LOP协议初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueLopPtlInit(void)
{
	unsigned int lCopCfg;
	gLopCanCfg.recvId1 = CAN_LOPDISP_ID|EXT_CANFRAME_TYPE;;
	gLopCanCfg.recvId1Mask = 0x00;
	gLopCanCfg.recvId2 = CAN_FDUPTBN_LIGHT_ID|EXT_CANFRAME_TYPE;
	gLopCanCfg.recvId2Mask = 0x0F;
	gLopCanCfg.recvError = 0;
	gLopCanCfg.sendError = 0;
	gLopCanCfg.ptFrame = &gRecvFrame;
	gLopCanCfg.recvIRQ = LeagueLopCanIrqHandle;
	DrvCanInit(40000,&gLopCanCfg); 
	gRecvFrameFlag = 0;
	gLopButtonChange = 0;
	gLopButtonSta = 0;
	
	lCopCfg = *((unsigned int*)LOP_WORK_CFG_ADR);
	if(lCopCfg == 0xFFFFFFFF){
		lCopCfg = 0x0101;
		setLeagueLopAdr(1,1);
	}
	gLopType = lCopCfg&0xFF;
	gLopFloorAdr = (lCopCfg>>8)&0xFF;
	
	if(gLopType == 0)
		gLopType = 1;
	else if(gLopType >2)
		gLopType = 1;
	
	if(gLopFloorAdr == 0)
		gLopFloorAdr = 1;
	else if(gLopFloorAdr > 64)
		gLopFloorAdr = 64;
}
/*!
 * LeagueLopPtlHandle
 * 联盟LOP协议处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueLopPtlHandle(void)
{
	DisplayFrameHandle();
	if(gLopType == 1){//前门
		FrontDoorUpBtnLightHandle();
		FrontDoorDnBtnLightHandle();
	}else if(gLopType == 2){//后门
		BackDoorUpBtnLightHandle();
		BackDoorDnBtnLightHandle();
	}
	ScanFrameHandle();
	LeagueLopReportButtonStatus();
}
/*!
 * FrontDoorUpBtnLightHandle
 * 前门上按钮灯处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void FrontDoorUpBtnLightHandle(void)
{
	unsigned char lBitPos;
	unsigned char lBufPos;
	if((gRecvFrameFlag&FRAME_FDUPBTN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_FDUPBTN_MASK;
	lBitPos = 1<<((gLopFloorAdr-1)%8);
	lBufPos = (gLopFloorAdr-1)/8;
	if(lBufPos > 7)
		return;
	if(gFdUpBtnFrame.Data[lBufPos]&lBitPos){
		//上按钮灯亮
		gLopButtonLight |= 0x01;
	}else{
		gLopButtonLight &= 0xFE;
		//上按钮灯灭
	}
}
/*!
 * FrontDoorDnBtnLightHandle
 * 前门下按钮灯处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void FrontDoorDnBtnLightHandle(void)
{
	unsigned char lBitPos;
	unsigned char lBufPos;	
	if((gRecvFrameFlag&FRAME_FDDNBTN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_FDDNBTN_MASK;
	lBitPos = 1<<((gLopFloorAdr-1)%8);
	lBufPos = (gLopFloorAdr-1)/8;
	if(lBufPos > 7)
		return;
	if(gFdDnBtnFrame.Data[lBufPos]&lBitPos){
		//下按钮灯亮
		gLopButtonLight |= 0x02;
	}else{
		//下按钮灯灭
		gLopButtonLight &= 0xFD;
	}
}
/*!
 * BackDoorUpBtnLightHandle
 * 后门上按钮灯处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BackDoorUpBtnLightHandle(void)
{
	unsigned char lBitPos;
	unsigned char lBufPos;		
	if((gRecvFrameFlag&FRAME_BDUPBTN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_BDUPBTN_MASK;
	lBitPos = 1<<((gLopFloorAdr-1)%8);
	lBufPos = (gLopFloorAdr-1)/8;
	if(lBufPos > 7)
		return;
	if(gBdUpBtnFrame.Data[lBufPos]&lBitPos){
		//上按钮灯亮
		gLopButtonLight |= 0x01;
	}else{
		gLopButtonLight &= 0xFE;
		//上按钮灯灭
	}
}
/*!
 * BackDoorDnBtnLightHandle
 * 后门下按钮灯处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BackDoorDnBtnLightHandle(void)
{
	unsigned char lBitPos;
	unsigned char lBufPos;	
	if((gRecvFrameFlag&FRAME_BDDNBTN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_BDDNBTN_MASK;
	lBitPos = 1<<((gLopFloorAdr-1)%8);
	lBufPos = (gLopFloorAdr-1)/8;
	if(lBufPos > 7)
		return;
	if(gBdDnBtnFrame.Data[lBufPos]&lBitPos){
		//下按钮灯亮
		gLopButtonLight |= 0x02;
	}else{
		//下按钮灯灭
		gLopButtonLight &= 0xFE;
	}
}
/*!
 * DisplayFrameHandle
 * 显示帧处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DisplayFrameHandle(void)
{
	unsigned char lValue;
	if((gRecvFrameFlag&FRAME_DISP_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_DISP_MASK;
	setSegLedShowArrow(gDispFrame.Data[1]&0x03);
	setSegLedShowOutOfService(gDispFrame.Data[1]&0x08);
	setSegLedShowFullload(gDispFrame.Data[5]&0x04);
	if(gDispFrame.Data[2]&0x80){//按照方式1显示3位楼层
		lValue = gDispFrame.Data[2] - 0x80;
		setSegLedShowThreeFloor(HexToAscii((lValue>>4)&0x0F),
								HexToAscii(lValue&0x0F),gDispFrame.Data[3]);
	}else if(gDispFrame.Data[3]&0x80){//按照方式2显示3位楼层
		lValue = gDispFrame.Data[3] - 0x80;
		setSegLedShowThreeFloor(gDispFrame.Data[2],HexToAscii((lValue>>4)&0x0F),
								HexToAscii(lValue&0x0F));	
	}else{//两位楼层显示
		setSegLedShowTwoFloor(gDispFrame.Data[2],gDispFrame.Data[3]);
	}
}
/*!
 * HexToAscii
 * 0-F Hex值转ASCII码
 * @param[in]   Hex值
 * @param[out]  None
 * @retval  	None
 */
unsigned char HexToAscii(unsigned char value)
{
	if(value < 10){
		return '0'+value;
	}else if(value < 16){
		return 'A'+(value-10);
	}
	return 0;
}
/*!
 * ScanFrameHandle
 * 控制柜扫描帧应答
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ScanFrameHandle(void)
{
	s_CANFRAME_T lScanFrame;
	if((gRecvFrameFlag&FRAME_SCAN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_SCAN_MASK;
	if(gScanFrame.Data[1] != gLopType)//非当前LOP类型
		return;
	if(gScanFrame.Data[0] != gLopFloorAdr)//楼层值不匹配
		return;	
	lScanFrame.Id = CAN_SCAN_RESPOND_ID;
	lScanFrame.Flag = 0x08| EXT_CANFRAME_TYPE;	
	lScanFrame.Data[0] = gScanFrame.Data[0];
	lScanFrame.Data[1] = gScanFrame.Data[1];
	lScanFrame.Data[2] = gScanFrame.Data[2];
	lScanFrame.Data[3] = *((unsigned char*)(CFG_FLASH_APP_TAGADR+TAG_PM_LENTH+3));
	lScanFrame.Data[4] = *((unsigned char*)(CFG_FLASH_APP_TAGADR+TAG_PM_LENTH+5));;
	lScanFrame.Data[5] = 0;
	lScanFrame.Data[6] = 0;
	lScanFrame.Data[7] = 0;
	DrvCanSendFrame(&lScanFrame);
}
/*!
 * LeagueLopReportButtonStatus
 * 上报按钮状态
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueLopReportButtonStatus(void)
{
	s_CANFRAME_T lBtnFrame;
	if(gLopButtonChange == 0)
		return;
	gLopButtonChange = 0;
	if(gLopType == 1){//前门
		lBtnFrame.Id = CAN_BASE_FDBTN_ID+gLopFloorAdr;
	}else{
		lBtnFrame.Id = CAN_BASE_BDBTN_ID+gLopFloorAdr;
	}
	lBtnFrame.Flag = 0x08| EXT_CANFRAME_TYPE;
	lBtnFrame.Data[0] = gLopButtonSta;
	lBtnFrame.Data[1] = 0;
	lBtnFrame.Data[2] = 0;
	lBtnFrame.Data[3] = 0;
	lBtnFrame.Data[4] = 0;
	lBtnFrame.Data[5] = 0;
	lBtnFrame.Data[6] = 0;
	lBtnFrame.Data[7] = 0;
	DrvCanSendFrame(&lBtnFrame);
}
/*!
 * LeagueLopCanIrqHandle
 * CAN中断处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueLopCanIrqHandle(void)
{
//	if((gRecvFrame.Flag&EXT_CANFRAME_TYPE) == 0)//非扩展帧
//		return;
	if(gRecvFrame.Id == CAN_LOPDISP_ID){
		gDispFrame = gRecvFrame;
		gRecvFrameFlag |= FRAME_DISP_SHIFT;
	}else if(gRecvFrame.Id == CAN_MASTER_SCAN_ID){
		gScanFrame = gRecvFrame;
		gRecvFrameFlag |= FRAME_SCAN_SHIFT;
	}else{
		if(gLopType == 1){//前门
			if(gRecvFrame.Id == CAN_FDUPTBN_LIGHT_ID){
				gFdUpBtnFrame = gRecvFrame;
				gRecvFrameFlag |= FRAME_FDUPBTN_SHIFT;
			}else if(gRecvFrame.Id == CAN_FDDNBTN_LIGHT_ID){
				gFdDnBtnFrame = gRecvFrame;
				gRecvFrameFlag |= FRAME_FDDNBTN_SHIFT;
			}	
		}else if(gLopType == 2){//后门
			if(gRecvFrame.Id == CAN_BDUPTBN_LIGHT_ID){
				gBdUpBtnFrame = gRecvFrame;
				gRecvFrameFlag |= FRAME_BDUPBTN_SHIFT;
			}else if(gRecvFrame.Id == CAN_BDDNBTN_LIGHT_ID){
				gBdDnBtnFrame = gRecvFrame;
				gRecvFrameFlag |= FRAME_BDDNBTN_SHIFT;
			}	
		}
	}
}
