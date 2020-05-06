/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: OtisLopPtlApp.c
	Author: ZhiweiZhang Version: V0 Date: 2019-10-24
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "OtisLopPtlApp.h"
#include "OtisHbRsl.h"
#include "RslHbGpio.h"
#include "OtisHpiRsl.h"
#include "DrvTimer.h"
#if NO_SUPORT_LCD_SHOW
#include "OtisLcd.h"
#endif
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#define SYS_CFG_HPISPEC_ADR	0x00002400	//HPI接口特殊显示(箭头&功能)帧地址存储在Flash中的位置
#define SYS_CFG_HBLOCK_ADR 	0x00002800	//HB接口锁梯帧地址存储在Flash中的位置


static unsigned char gCurHbPortAdress;	//当前HB接口地址
static unsigned char gLastLockStatus;	//上一次锁梯状态
static unsigned char gLastRecevData;	//上一次HB接口的接收数据
static unsigned char gLastSendData;		//上一次HB接口的发送数据


#define SEG_FILTER_SIZE	1
static unsigned char gSpecSegBuf[SEG_FILTER_SIZE];	//特殊显示段缓冲区
static unsigned char gSpecStableSeg;	//滤波后可用于显示的特殊数据段
static unsigned char gSegBufFilter;	//滤波计数
static unsigned char gCurHpiAddress;	//当前的HPI箭头显示地址
static unsigned char gHpiFrameBuf[64];	//HPI帧数据缓冲区
static unsigned long gFloorSegBuf[SEG_FILTER_SIZE];	//楼层显示段缓冲区
static unsigned long gFloorStableSeg;	//滤波后可用于显示的楼层段数据
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void HbPortAdressHandle(void);
void HbPortButtonHandle(void);
void HbLockButtonHandle(void);
extern void ZkMemCpy(void *dst,void *src,const unsigned long len);
void OtisRslSegFilterHandle(void);
unsigned long GetFloorSeg(void);
unsigned char GetSpecSeg(void);
void OtisHpiReadData(void);
#if NO_SUPORT_LCD_SHOW
void OtisRslSpecDataHandle(void);
void OtisRslFloorSegHandle(void);
#endif
void CleanHpiFrame(void);
//**
// * @}
/*!
 * OtisRslGetFloorSeg
 * 获取楼层显示段位
 * @param[in]   None
 * @param[out]  None
 * @retval  	楼层段位
 */
unsigned long OtisRslGetFloorSeg(void)
{
	return gFloorStableSeg;
}
/*!
 * OtisRslGetLiftStatus
 * 获取电梯运行状态
 * @param[in]   None
 * @param[out]  None
 * @retval  	0：无运行	非0：运行中
 */
unsigned char OtisRslGetLiftStatus(void)
{
	return gSpecStableSeg;
} 
/*!
 * OtisLopPtlAppInit
 * LOP协议应用初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void OtisLopPtlAppInit(void)
{
	unsigned char lLockAdr;
	//HB接口初始化
	OtisHbRslInit();
	RslHbGpioInit();
	gCurHbPortAdress = GetHbGpioDevAdr();
	if(gCurHbPortAdress < 4){
		SetHbPortSendData(0);
		SetHbDnButtonStatus(RSL_LIGHT_OFF);
		SetHbUpButtonStatus(RSL_LIGHT_OFF);
	}else{	
		SetHbPortAddress(gCurHbPortAdress);
	}
	//HB锁梯初始化
	lLockAdr = *((unsigned char *)SYS_CFG_HBLOCK_ADR);//从Flash上加载锁梯帧地址
	if((lLockAdr < 68)||(lLockAdr > 127))//非法地址
		lLockAdr = 124;//设定为默认地址
	SetHbLockAddress(lLockAdr);
	gLastLockStatus = GetHbGpioKeyStatus(RSL_LOCKBTN_KEY);
	if(gLastLockStatus){
		SetHbLockSendData(0x11);
	}else{
		SetHbLockSendData(0);
	}
	gLastRecevData = 0;
	gLastSendData = 0;
	//HPI接口初始化
	OtisHpiRslInit();
	gCurHpiAddress = *((unsigned char *)SYS_CFG_HPISPEC_ADR);//从Flash上加载HPI特殊显示帧地址
	if(gCurHpiAddress > 63)//非法地址
		gCurHpiAddress = 56;//设定为默认地址
	gFloorStableSeg = 0;
	gSpecStableSeg = 0;	
}
/*!
 * OtisLopPtlAppHandle
 * LOP协议应用处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void OtisLopPtlAppHandle(void)
{	
	RslHbGpioInputHandle();
	HbPortAdressHandle();
	HbPortButtonHandle();
	HbLockButtonHandle();
	OtisHpiReadData();
	if(GetHpiTimeOut()){//显示通讯超时
		gFloorStableSeg = 0;
		gSpecStableSeg = 0;
		HpiReceiveBufferClean();
		CleanHpiFrame();
	}
#if NO_SUPORT_LCD_SHOW 	
	OtisRslSpecDataHandle();
	OtisRslFloorSegHandle();
#endif	
}
/*!
 * CleanHpiFrame
 * LOP协议应用处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void CleanHpiFrame(void)
{
	unsigned char lCnt;
	
	for(lCnt = 0; lCnt < 64; lCnt++){
		gHpiFrameBuf[lCnt] = 0;
	}
}
/*!
 * OtisRslSpecDataHandle
 * OTIS特殊帧(箭头/功能)数据处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
#if NO_SUPORT_LCD_SHOW 
void OtisRslSpecDataHandle(void)
{	
	OtisRslLcdStopSegShow(gSpecStableSeg&0x01);
	OtisRslLcdFullSegShow(gSpecStableSeg&0x02);
	if(gSpecStableSeg&0x04){
		OtisRslLcdArrowUpSegShow(1);//上行
	}else if(gSpecStableSeg&0x08){
		OtisRslLcdArrowDnSegShow(1);//下行		
	}else{
		OtisRslLcdArrowUpSegShow(0);//上行
		OtisRslLcdArrowDnSegShow(0);//下行
	}
}
#endif
/*!
 * OtisRslFloorSegHandle
 * OTIS楼层段数据处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
#if NO_SUPORT_LCD_SHOW 
void OtisRslFloorSegHandle(void)
{
	unsigned char lCnt;
	unsigned char lSegH = 0;
	unsigned char lSegL = 0;
	for(lCnt = 0; lCnt < 16; lCnt++){
		if(gFloorStableSeg&(0x10000<<lCnt)){
			lSegH = 1;
		}else{
			lSegH = 0;
		}
		if(gFloorStableSeg&(1<<lCnt)){
			lSegL = 1;
		}else{
			lSegL = 0;
		}
		OtisRslLcdFloorShowSeg(lCnt,lSegL,lSegH);	
	}
}
#endif
/*!
 * OtisHpiReadData
 * 读取HPI数据
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void OtisHpiReadData(void)
{
	unsigned char *ptDat;	
	
	ptDat = GetHpiReceiveData();
	if(ptDat == 0)
		return;
	ZkMemCpy(gHpiFrameBuf,ptDat,64);
	OtisRslSegFilterHandle();
}
/*!
 * OtisRslSegFilterHandle
 * 段数据简易滤波处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void OtisRslSegFilterHandle(void)
{
	unsigned char lCnt;
	unsigned char lIsSpecSame = 1;
	unsigned char lIsFloorSame = 1;
	unsigned char lSize;
	
	gFloorSegBuf[gSegBufFilter] = GetFloorSeg();
	gSpecSegBuf[gSegBufFilter] = GetSpecSeg();
	gSegBufFilter++;
	if(gSegBufFilter < SEG_FILTER_SIZE)
		return;
	gSegBufFilter = 0;
	lSize = SEG_FILTER_SIZE - 1;
	for(lCnt = 0; lCnt < lSize; lCnt++){
		if(gFloorSegBuf[lCnt] != gFloorSegBuf[lCnt+1]){
			lIsFloorSame = 0;
		}
		if(gSpecSegBuf[lCnt] != gSpecSegBuf[lCnt+1]){
			lIsSpecSame = 0;
		}		
	}
	if(lIsFloorSame)
		gFloorStableSeg = gFloorSegBuf[lSize];
	if(lIsSpecSame)
		gSpecStableSeg = gSpecSegBuf[lSize];
}
/*!
 * GetFloorSeg
 * 获取楼层段数据
 * @param[in]   None
 * @param[out]  None
 * @retval  	高16位-高位楼层段	低16位-低位楼层段
 */
unsigned long GetFloorSeg(void)
{
	unsigned long lSeg = 0;
	unsigned char lCnt;
	unsigned char lStartPos = 6;
	
	for(lCnt = 0; lCnt < 16; lCnt++){
		if(lCnt > 7){
			lStartPos = 22;
		}
		if(gHpiFrameBuf[lStartPos+lCnt]&0x10){
			lSeg |= 1<<lCnt;
		}
		if(gHpiFrameBuf[14+lCnt]&0x10){
			lSeg |= 0x10000<<lCnt;
		}	
	}
	return lSeg;
}
/*!
 * GetSpecSeg
 * 获取特殊数据段
 * @param[in]   None
 * @param[out]  None
 * @retval  	特殊数据段内容
 */
unsigned char GetSpecSeg(void)
{
	unsigned char lSpec = 0;

	if(gCurHpiAddress == 60){
		lSpec = (gHpiFrameBuf[gCurHpiAddress]&0x03)<<2;
		if(gHpiFrameBuf[gCurHpiAddress]&0x04)
			lSpec |= 0x02;
		if(gHpiFrameBuf[gCurHpiAddress]&0x08)
			lSpec |= 0x01;
		return lSpec;
	}
	return gHpiFrameBuf[gCurHpiAddress];
}
/*!
 * HbLockButtonHandle
 * 锁梯按钮处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HbLockButtonHandle(void)
{
	unsigned char lSta;
	
	lSta = GetHbGpioKeyStatus(RSL_LOCKBTN_KEY);
	if(gLastLockStatus == lSta)
		return;
	gLastLockStatus = lSta;
	if(gLastLockStatus){
		SetHbLockSendData(0x11);
	}else{
		SetHbLockSendData(0);
	}	
}
/*!
 * HbPortButtonHandle
 * LOP按钮处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HbPortButtonHandle(void)
{
	unsigned char lHbRecv;
	unsigned char lHbSend;
	
	if(gCurHbPortAdress < 4){//不属于有效地址
		return;
	}
	lHbRecv = GetHbPortReceiveData();
	if(lHbRecv != gLastRecevData){
		gLastRecevData = lHbRecv;
		if(lHbRecv&0x1){//上行灯亮
			SetHbUpButtonStatus(RSL_LIGHT_ON);
		}else{
			SetHbUpButtonStatus(RSL_LIGHT_OFF);
		}
		if(lHbRecv&0x2){//下行灯亮
			SetHbDnButtonStatus(RSL_LIGHT_ON);
		}else{
			SetHbDnButtonStatus(RSL_LIGHT_OFF);
		}		
	}
	//发送按钮信息处理
	lHbSend = GetHbButtonValue();
	if(lHbSend != gLastSendData){
		gLastSendData = lHbSend;
		lHbSend = 0;
		if(gLastSendData&RSL_UPBTN_KEY){
			lHbSend |= 0x11;
		}
		if(gLastSendData&RSL_DNBTN_KEY){//下按钮按下
			if(lHbSend == 0){//上按钮未按下
				lHbSend |= 0x12;//Bit5校验位为1
			}else{
				lHbSend = 0x03;//Bit5校验位为0
			}
		}
		SetHbPortSendData(lHbSend);		
	}
}
/*!
 * HbPortAdressHandle
 * HB接口地址处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void HbPortAdressHandle(void)
{
	unsigned char lHbPortAdr;
	
	lHbPortAdr = GetHbGpioDevAdr();
	if(lHbPortAdr != gCurHbPortAdress){
		gCurHbPortAdress = lHbPortAdr;
		if(gCurHbPortAdress < 4){
			SetHbPortSendData(0);
			SetHbDnButtonStatus(RSL_LIGHT_OFF);
			SetHbUpButtonStatus(RSL_LIGHT_OFF);
		}else{
			SetHbPortAddress(gCurHbPortAdress);
		}
	}	
}
