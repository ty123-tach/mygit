/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: CopBtnPtl.c
	Author: ZhiweiZhang Version: V0 Date: 2020-01-07
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "CopBtnPtl.h"
#include "DrvUart0.h"
#include "MKE16Z4.h"
#include "DrvTimer.h"
#include "DebugMsg.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#define AUTO_SEND_SPACE 100	//自动发送间隔，单位ms
static unsigned int gCheckTimeStamp = 0;		//从机检测时间戳
static unsigned int gSysSendTimeStamp = 0;		//发送时间戳

static unsigned char gSysSendBuf[22] = {0};	//发送数据缓冲区
static unsigned char gSysRecvBuf[22] = {0}; //接收缓冲区
static unsigned char gSysSendIndex = 0;	//当前发送索引
static unsigned char gSysRecvIndex = 0;	//当前接收索引
static unsigned char gSysRecvFinished = 0;

static unsigned char gUserCmdBuf[2][20] = {0};	//用户命令缓冲区
static unsigned char gUserCmd = 0;
static unsigned char gSlaveKeyMap[9][8]; //从机密钥映射表
static unsigned char gSlaveValidMap[72];//从机灯有效映射,0：1-点亮 0-灭灯 1：1-灭灯 0-亮灯
#define MAX_SLAVE_KEYPLUSE	3
static unsigned char gSlaveKeyPluse[72];//从机按键脉冲计数
static unsigned char gSlaveKeyStatus[9];//从机按键状态

#define MAX_SALVENUM_SUPPORT 72		//最大支持的从机密钥个数
#define MAX_IDCNT_ERROR	5			//从机密钥错误最大计数
#define MAX_SLAVE_CHECKTIME	300000	//从机密钥检测间隔单位ms 5*60*1000
static unsigned char gCurQuerySlave = 0;
static unsigned char gSlaveIdError[72];//从机密钥错误计数
static unsigned char gSlaveInValid[9];//从机无效状态 1；无效 0；有效
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void AutoSendDataHandle(void); 
unsigned char GetXorValue(unsigned char index);
void CmdBufClean(unsigned char index);
void SlaveKeyMapInit(void);
void SlaveKeyStatusHandle(void);
void SlaveKeyMapHandle(void);
void CheckSlaveKeyMapHandle(void);
void SlaveAdrSetHandle(void);
//**
// * @}
/*!
 * BtnPtlRWCmdInit
 * 按钮协议读写命令初始化
 * @param[in]   mode：模式 0-按钮无需自保持	1-按钮需自保持
 * @param[out]  None
 * @retval  	None
 */
void BtnPtlRWCmdInit(unsigned char mode)
{
	unsigned char lCnt;
	unsigned char lByteAdr,lBitAdr;
	
	CmdBufClean(0);
	if(mode){
		gUserCmdBuf[0][0] = 0x1F;
	}else{
		gUserCmdBuf[0][0] = 0x0F;
	}
	for(lCnt = 0; lCnt < 72; lCnt++){
		lByteAdr = lCnt/8+1;
		lBitAdr = lCnt%8;
		if(gSlaveValidMap[lCnt]){//奇数
			gUserCmdBuf[0][lByteAdr] |= 1<<lBitAdr;
		}
	}
	gUserCmdBuf[0][10] = GetXorValue(0);
	gUserCmd = 0;
}
/*!
 * BtnPtlReadIdCmd
 * 按钮协议读取从机ID命令
 * @param[in]   adr：从机地址 范围0-71
 * @param[out]  None
 * @retval  	None
 */
void BtnPtlReadIdCmd(unsigned char adr)
{
	unsigned int CurrentTime;
	CmdBufClean(1);
	gUserCmdBuf[1][0] = 0x56;
	gUserCmdBuf[1][1] = adr/8+11;
	gUserCmdBuf[1][2] = adr%8;
	CurrentTime = DrvGetTimerMsTick();
	gUserCmdBuf[1][5] = CurrentTime&0xFF;
	gUserCmdBuf[1][3] = (CurrentTime>>8)&0xFF;
	gUserCmdBuf[1][4] = (CurrentTime>>16)&0xFF;
	gUserCmdBuf[1][6] = (CurrentTime>>24)&0xFF;
	gUserCmdBuf[1][10] = GetXorValue(1);
	gUserCmd = 1;	
}
/*!
 * BtnPtlAdrSetCmd
 * 按钮地址设定命令
 * @param[in]   adr：当前需要设定的地址
 * @param[out]  None
 * @retval  	None
 */
void BtnPtlAdrSetCmd(unsigned char adr)
{
	unsigned char ByteAdr,BitAdr;
	unsigned char CurrentTime;
	unsigned char Temp;
	if((adr > 72)||(adr < 1))
		return;
	ByteAdr = (adr-1)/8;
	BitAdr = (adr-1)%8;
	CurrentTime = DrvGetTimerMsTick()&0xFF;
	CmdBufClean(1);	
	gUserCmdBuf[1][0] = 0x5A;
	if(gSlaveValidMap[adr-1]){//加法计算地址
		gUserCmdBuf[1][1] = 0;
		Temp = ByteAdr - CurrentTime;
		gUserCmdBuf[1][2] = Temp;
		gUserCmdBuf[1][4] = CurrentTime;
		Temp = BitAdr - CurrentTime;
		gUserCmdBuf[1][3] = Temp;
		gUserCmdBuf[1][5] = CurrentTime;		
	}else{//减法计算地址
		gUserCmdBuf[1][1] = 1;
		Temp = CurrentTime + ByteAdr;
		gUserCmdBuf[1][2] = Temp;
		gUserCmdBuf[1][4] = CurrentTime;	
		Temp = CurrentTime + BitAdr;		
		gUserCmdBuf[1][3] = Temp;
		gUserCmdBuf[1][5] = CurrentTime;
	}
	gUserCmdBuf[1][10] = GetXorValue(1);
	gUserCmd = 1;
}
/*!
 * CopBtnPtlInit
 * COP按钮协议初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CopBtnPtlInit(void)
{
	DrvUart0Init();
	SlaveKeyMapInit();
	gSysSendTimeStamp = DrvGetTimerMsTick();
	gSysSendIndex = 0;
	gSysRecvIndex = 0;
	gSysRecvFinished = 0;
	BtnPtlRWCmdInit(1);
	gUserCmd = 0;
	gCurQuerySlave = 0;
	gCheckTimeStamp = DrvGetTimerMsTick();
}
/*!
 * CopBtnPtlHandle
 * COP按钮协议处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CopBtnPtlHandle(void)
{
	SlaveAdrSetHandle();
	CheckSlaveKeyMapHandle();
	SlaveKeyMapHandle();
	SlaveKeyStatusHandle();
	AutoSendDataHandle();
}
/*!
 * SlaveAdrSetHandle
 * 从机地址设定响应处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SlaveAdrSetHandle(void)
{
	unsigned char lCnt,lIndex;
	unsigned char lData;
	unsigned char lShift;	
	if(gSysRecvFinished == 0)//未接收完一帧数据
		return;
	if(gSysRecvBuf[0] != 0x5A)//非地址设定帧
		return;
	gSysRecvFinished = 0;
	for(lCnt = 0; lCnt < 9; lCnt++){
		lData = gSysRecvBuf[11+lCnt];
		for(lIndex = 0; lIndex < 8; lIndex++){
			lShift = 1<<lIndex;
			if(lData&lShift){//按键有按下
				gSlaveKeyStatus[lCnt] |= lShift;
			}else{
				gSlaveKeyStatus[lCnt] &= ~lShift;
			}
		}
	}	
}
/*!
 * BtnPtlGetKeyStatus
 * 获取从机按键状态
 * @param[in]  	adr：从机地址
 * @param[out]  None
 * @retval  	None
 */
unsigned char BtnPtlGetKeyStatus(unsigned char adr)
{
	unsigned char ByteAdr,BitAdr;
	
	if(adr > 71)
		return 0;
	ByteAdr = adr/8;
	BitAdr = adr%8;
	if(gSlaveInValid[ByteAdr]&(1<<BitAdr))//按钮无效
		return 0;
	return gSlaveKeyStatus[ByteAdr]&(1<<BitAdr);
}
/*!
 * BtnPtlKeyStatusBuf
 * 获取从机按键状态缓冲区
 * @param[in]  	None
 * @param[out]  None
 * @retval  	缓冲区地址
 */
unsigned char *BtnPtlKeyStatusBuf(void)
{
	return gSlaveKeyStatus;
}
/*!
 * BtnPtlSetLightStatus
 * 设置按钮灯状态
 * @param[in]  	adr：从机地址	sta：从机灯状态
 * @param[out]  None
 * @retval  	None
 */
void BtnPtlSetLightStatus(unsigned char adr,unsigned char sta)
{
	unsigned char ByteAdr,BitAdr;
	if(adr > 71)
		return;
	ByteAdr = adr/8;
	BitAdr = adr%8;
	if(gSlaveInValid[ByteAdr]&(1<<BitAdr))//按钮无效
		return;	
	ByteAdr += 1;
	gUserCmdBuf[0][0] = 0x1F;
	if(gSlaveValidMap[adr]){//奇数,0点灯 1灭灯
		if(sta){//亮灯
			gUserCmdBuf[0][ByteAdr] &= ~(1<<BitAdr);
		}else{
			gUserCmdBuf[0][ByteAdr] |= (1<<BitAdr);
		}	
	}else{//偶数,1点灯,0灭灯
		if(sta){//亮灯
			gUserCmdBuf[0][ByteAdr] |= (1<<BitAdr);
		}else{
			gUserCmdBuf[0][ByteAdr] &= ~(1<<BitAdr);
		}		
	}
	gUserCmdBuf[0][10] = GetXorValue(0);
	gUserCmd = 0;	
}
/*!
 * CheckSlaveKeyMapHandle
 * 从机密钥处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CheckSlaveKeyMapHandle(void)
{
	if((DrvGetTimerMsTick()-gCheckTimeStamp) < MAX_SLAVE_CHECKTIME)
		return;
	BtnPtlReadIdCmd(gCurQuerySlave);
}
/*!
 * SlaveKeyMapHandle
 * 从机密钥映射处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SlaveKeyMapHandle(void)
{
	unsigned char ByteAdr;
	unsigned char BitAdr;
	if(gSysRecvFinished == 0)//未接收完一帧数据
		return;
	if(gSysRecvBuf[0] != 0x56)//非密钥处理帧
		return;
	gSysRecvFinished = 0;
	ByteAdr = gCurQuerySlave/8;
	BitAdr = gCurQuerySlave%8;	
	if(gSysRecvBuf[15] != gSlaveKeyMap[ByteAdr][BitAdr]){//密钥不正确
		gSlaveIdError[gCurQuerySlave]++;
		if(gSlaveIdError[gCurQuerySlave] > MAX_IDCNT_ERROR){//超过错误次数
			gSlaveIdError[gCurQuerySlave] = MAX_IDCNT_ERROR;
			gSlaveInValid[ByteAdr] |= 1<<BitAdr;//标记从机无效
		}
	}else{
		gSlaveIdError[gCurQuerySlave] = 0;
		gSlaveInValid[ByteAdr] &= ~(1<<BitAdr);
	}
	gCurQuerySlave++;
	if(gCurQuerySlave >= MAX_SALVENUM_SUPPORT){
		gCurQuerySlave =0;
	}	
	gCheckTimeStamp = DrvGetTimerMsTick();
	gUserCmd = 0;//切换回常规按钮读取指令	
}
/*!
 * SlaveKeyStatusHandle
 * 从机按钮状态处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SlaveKeyStatusHandle(void)
{
	unsigned char lCnt,lIndex;
	unsigned char lData;
	unsigned char lPluse;
	unsigned char lShift;
	if(gSysRecvFinished == 0)//未接收完一帧数据
		return;
	if(gSysRecvBuf[0] != 0x1F)//非正常按钮帧
		return;
	gSysRecvFinished = 0;
	for(lCnt = 0; lCnt < 9; lCnt++){
		lData = gSysRecvBuf[11+lCnt];
		lPluse = lCnt*8;
		for(lIndex = 0; lIndex < 8; lIndex++){
			lShift = 1<<lIndex;
			if(lData&lShift){//按键有按下
				if((gSlaveInValid[lCnt]&lShift) == 0){//按钮有效
					gSlaveKeyPluse[lPluse]++;
					if(gSlaveKeyPluse[lPluse] > MAX_SLAVE_KEYPLUSE){
						gSlaveKeyPluse[lPluse] = MAX_SLAVE_KEYPLUSE;
						gSlaveKeyStatus[lCnt] |= lShift;
					}
				}
			}else{
				gSlaveKeyStatus[lCnt] &= ~lShift;
				gSlaveKeyPluse[lPluse] = 0;
			}
			lPluse++;
		}
	}
}
/*!
 * AutoSendDataHandle
 * 串口自动发送数据处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void AutoSendDataHandle(void)
{	
	unsigned char lCnt;
	if(((DrvGetTimerMsTick()-gSysSendTimeStamp) < AUTO_SEND_SPACE)||(gSysRecvFinished == 1)){
		if(gSysRecvFinished){
			if((DrvGetTimerMsTick()-gSysSendTimeStamp) > 3000)//3S未处理，切换
				gSysRecvFinished = 0;
		}
		return;
	}
	gSysSendTimeStamp = DrvGetTimerMsTick();
	gSysSendIndex = 0;
	gSysRecvIndex = 0;
	for(lCnt = 0; lCnt < 20; lCnt++){
		gSysSendBuf[lCnt] = gUserCmdBuf[gUserCmd][lCnt];
//		DebugMsg("%02X ",gSysRecvBuf[lCnt]);
	}
//	DebugMsg("\r\n");
	LPUART0->FIFO |= LPUART_FIFO_RXFLUSH_SHIFT|LPUART_FIFO_TXFLUSH_SHIFT;
	LPUART0->CTRL |= LPUART_CTRL_TIE_MASK;	
}
/*!
 * LPUART0_IRQHandler
 * 串口0中断
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LPUART0_IRQHandler(void)
{
	uint32_t status;
	status = LPUART0->STAT;
	if(status&LPUART_STAT_RDRF_MASK){//接收中断
		gSysRecvBuf[gSysRecvIndex] = LPUART0->DATA;
		gSysRecvIndex++;
		if(gSysRecvIndex >= 20){
			gSysRecvIndex = 20;
			gSysRecvFinished = 1;
		}
	}else if(status&LPUART_STAT_TDRE_MASK){//发送中断
		if(gSysSendIndex < 20){
			LPUART0->DATA = gSysSendBuf[gSysSendIndex];
			gSysSendIndex++;	
			gSysSendTimeStamp = DrvGetTimerMsTick();
		}else{
			LPUART0->CTRL = (LPUART0->CTRL & (~LPUART_CTRL_TIE_MASK));
		}	
	}	
}
/*!
 * GetXorValue
 * 获取异或校验结果
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
unsigned char GetXorValue(unsigned char index)
{
	unsigned char lCnt;
	unsigned char lRes = 0;

	for(lCnt = 0; lCnt < 10;lCnt++){
		lRes = lRes ^ gUserCmdBuf[index][lCnt];
	}
	return lRes;
}
/*!
 * CmdBufClean
 * 清空命令缓冲区
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CmdBufClean(unsigned char index)
{
	unsigned char lCnt;
	for(lCnt = 0; lCnt < 20; lCnt++){
		gUserCmdBuf[index][lCnt] = 0;
	}
}
/*!
 * SlaveKeyMapInit
 * 从机键值映射初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SlaveKeyMapInit(void)
{
	unsigned char lTable[8] = {0xa7,0xd9,0x29,0x5a,0x8b,0x61,0xc3,0xf4};
	unsigned char lCnt,i,lSum;
	unsigned char *ptKey;

	for(lCnt = 0; lCnt < 9;lCnt++){
		for(i = 0; i < 8;i++){
			gSlaveKeyMap[lCnt][i] = lTable[i]-i+lCnt;
		}
	}
	ptKey = gSlaveKeyMap[0];
	for(lCnt = 0; lCnt < 72;lCnt++){
		lSum = 0;
		for(i = 0; i < 8; i++){
			if(*ptKey&(1<<i)){
				lSum++;
			}
		}
		if(lSum&0x01){//奇数
			gSlaveValidMap[lCnt] = 1;
		}else{//偶数
			gSlaveValidMap[lCnt] = 0;
		}
		ptKey++;
	}
} 
