/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: ZkPtlLowlevel.c
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "ZkPtlLowlevel.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#define MAX_LOWRECV_BUFSIZE	2	/*最大的底层数据接收缓冲区大小*/
#define MAX_PTLDATA_LENTH	144	/*一包协议数据最大的可能长度*/
#define SRC_SENDBUF_INDEX	0
#define DST_SENDBUF_INDEX	1
static unsigned char gLowSendBuf[2][MAX_PTLDATA_LENTH];
static unsigned char gLowRecvBuf[MAX_LOWRECV_BUFSIZE][MAX_PTLDATA_LENTH];
static unsigned char gLowRecvLen[MAX_LOWRECV_BUFSIZE];
static unsigned char gLowUseBuf;/*当前接收使用的缓冲区*/
static unsigned char gLowUsePos;/*当前接收使用的缓冲区数据位置*/
static unsigned char gLowHandleBuf;/*当前被处理的缓冲区*/
#define STA_IS_UNKNOW 0		/*未知状态*/
#define STA_IS_RECEIVE	1	/*正在接收状态*/
static unsigned char gLowRecvStatus;//接收状态
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
unsigned long ZkPtlRecoverFrame(unsigned char *src,unsigned long srclen);
unsigned long ZkPtlTransformFrame(void *src,unsigned long srclen,void *dst);
unsigned char ZkPtltVerify(unsigned char *data, unsigned long length);
extern void ZkPtlHwSendData(void *dat,unsigned long size);
//**
// * @}
/*!
 * ZkPtlLowlevelInit
 * 底层初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlLowlevelInit(void)
{
	gLowRecvStatus = STA_IS_UNKNOW;
	gLowHandleBuf = 0;
	gLowUseBuf = 0;
	gLowUsePos = 0;
}
/*!
 * ZkPtlLowlevelRecCh
 * 底层数据接收
 * @param[in]   ch：接收的字节数据
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlLowlevelRecCh(unsigned char ch)
{
	if(ch == ZK_STX){//接收到开始字符
		gLowRecvStatus = STA_IS_RECEIVE;
		gLowUsePos = 0;
		gLowRecvBuf[gLowUseBuf][gLowUsePos++] = ch;
	}else if(ch == ZK_ETX){//接收到结束字符
		gLowRecvBuf[gLowUseBuf][gLowUsePos++] = ch;
		gLowRecvLen[gLowUseBuf] = gLowUsePos;
		/*切换接收缓冲区*/
		gLowUseBuf++;
		if(gLowUseBuf >= MAX_LOWRECV_BUFSIZE)
			gLowUseBuf = 0;
		gLowRecvStatus = STA_IS_UNKNOW;
	}else{
		if(gLowRecvStatus != STA_IS_RECEIVE)//非接收状态
			return;
		if(gLowUsePos < MAX_PTLDATA_LENTH)//缓冲区未满
			gLowRecvBuf[gLowUseBuf][gLowUsePos++] = ch;
	}
}
/*!
 * ZkPtlLowlevelSendData
 * 底层发送数据
 * @param[in]   dat：待发送的数据
				len：待发送的数据长度
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlLowlevelSendData(void *dat,unsigned char tag,unsigned long len)
{
	unsigned long TranLen = 0;/*转换后帧长度*/
	unsigned char *ptSend = gLowSendBuf[SRC_SENDBUF_INDEX];

	*ptSend++ = ZK_STX;	/*加入帧头*/
	*ptSend++ = tag;	/*加入标签*/
	ZkMemCpy(ptSend,dat,len);/*加入数据*/
	ptSend += len;
	*ptSend++ = ZkPtltVerify(&gLowSendBuf[SRC_SENDBUF_INDEX][1],len+1);/*计算校验*/
	*ptSend++ = ZK_ETX;/*加入帧尾*/
	TranLen = ZkPtlTransformFrame(gLowSendBuf[SRC_SENDBUF_INDEX],
					ptSend-gLowSendBuf[SRC_SENDBUF_INDEX],gLowSendBuf[DST_SENDBUF_INDEX]);/*帧转换*/
	ZkPtlHwSendData(gLowSendBuf[DST_SENDBUF_INDEX],TranLen);
}
/*!
 * ZkPtlLowlevelSendBstData
 * 底层发送BST协议数据
 * @param[in]   dat：待发送的数据
				tag：待发送的数据标签
				len：待发送的数据长度
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlLowlevelSendBstData(void *dat,unsigned char tag,unsigned char len)
{
	unsigned char *ptSend = gLowSendBuf[SRC_SENDBUF_INDEX];
	
	*ptSend++ = ZK_STX;	/*加入帧头*/
	*ptSend++ = tag;	/*加入标签*/
	ZkMemCpy(ptSend,dat,len);/*加入数据*/
	ptSend += len;
	*ptSend++ = ZkPtltVerify(&gLowSendBuf[SRC_SENDBUF_INDEX][1],len+1);/*计算校验*/
	*ptSend++ = ZK_ETX;/*加入帧尾*/	
	ZkPtlHwSendData(gLowSendBuf[SRC_SENDBUF_INDEX],len+4);
}
/*!
 * ZkPtlGetLowlevelFrame
 * 底层获取帧数据
 * @param[in]   None
 * @param[out]  None
 * @retval  	帧数据地址
 */
unsigned char *ZkPtlGetLowlevelFrame(void)
{
	unsigned long TranFrameLen = 0;	/*转换后长度*/
	unsigned char temp;
	unsigned char *Frame;
	
	if(gLowHandleBuf == gLowUseBuf)//无帧数据
		return ZK_EMPTY_FRAME;
	temp = gLowHandleBuf;
	TranFrameLen = ZkPtlRecoverFrame(gLowRecvBuf[temp],gLowRecvLen[temp]);/*帧转换*/
	if(gLowRecvBuf[temp][TranFrameLen-2] != 
		ZkPtltVerify(&gLowRecvBuf[temp][1],TranFrameLen-3)){/* 数据校验失败 */	
		Frame = ZK_EMPTY_FRAME;
	}else{
		Frame = gLowRecvBuf[temp];
	}
	gLowHandleBuf++;
	if(gLowHandleBuf >= MAX_LOWRECV_BUFSIZE)
		gLowHandleBuf = 0;
	return Frame;
}
/*!
 * ZkPtlRecoverFrame
 * 协议去除字符转换，恢复数据
 * @param[in]   src：原数据地址 len：原数据长度
 * @param[out]  None
 * @retval  	恢复后的帧长度
 */
unsigned long ZkPtlRecoverFrame(unsigned char *src,unsigned long srclen)
{
	unsigned long Len = srclen-2;/*去掉帧头和帧尾*/
	unsigned long TranLen = 2;/*恢复后长度,帧头和帧尾*/
	unsigned long Cnt = 0;
	unsigned char *lDst = src;
	unsigned char *lSrc = src;
	
	*lDst++ = *lSrc++;/*取出帧头数据*/
	while(Cnt < Len)
	{
		if(*lSrc == ZK_ESC)
		{
			if(*(lSrc+1) == ZK_STX_AF)//0xE7
				*lDst++ = ZK_STX;
			else if(*(lSrc+1) == ZK_ETX_AF)//0xE8
				*lDst++ = ZK_ETX;
			else if(*(lSrc+1) == ZK_ESC_AF)//0x00
				*lDst++ = ZK_ESC;
			lSrc += 2;
			Cnt += 2;
		}else{
			*lDst++ = *lSrc++;
			Cnt++;
		}
		TranLen++;
	}
	*lDst = *lSrc;/*取出帧尾数据*/
	return TranLen;
}
/*!
 * ZkPtlTransformFrame
 * 协议转换帧数据
 * @param[in]   src：原数据地址 dst：转换后数据地址 
				len：原数据长度
 * @param[out]  None
 * @retval  	返回转换后的长度
 */
unsigned long ZkPtlTransformFrame(void *src,unsigned long srclen,void *dst)
{
	unsigned long Len = srclen-2;/*去掉帧头和帧尾*/
	unsigned long TranLen = 2;
	unsigned char Cnt = 0;
	unsigned char *lDst = (unsigned char*)dst;
	unsigned char *lSrc = (unsigned char*)src;
	
	*lDst++ = *lSrc++;/*取出帧头数据*/
	while(Cnt < Len)
	{
		if(*lSrc == ZK_STX)/*遇到帧头数据*/
		{
			*lDst++ = ZK_ESC;
			*lDst++ = ZK_STX_AF;
			TranLen += 2;
		}else if(*lSrc == ZK_ETX){/*遇到帧尾数据*/
			*lDst++ = ZK_ESC;
			*lDst++ = ZK_ETX_AF;
			TranLen += 2;
		}else if(*lSrc == ZK_ESC){/*遇到转换字符*/
			*lDst++ = ZK_ESC;
			*lDst++ = ZK_ESC_AF;
			TranLen += 2;
		}else{
			*lDst++ = *lSrc;
			TranLen++;
		}
		lSrc++;
		Cnt++;	
	}	
	*lDst = *lSrc;/*取出帧尾数据*/
	return TranLen;
}
/*!
 * ZkPtltVerify
 * 协议校验算法
 * @param[in]   data：待校验的数据指针	length：待校验的数据长度
 * @param[out]  None
 * @retval  	None
 */
unsigned char ZkPtltVerify(unsigned char *data, unsigned long length)
{
	unsigned char result = 0;
	unsigned int i;
	
	for (i = 0; i < length; i++){
		result = result ^ data[i];
	}	
	return result & 0x7f;
}
/*!
 * ZkMemCpy
 * 内存拷贝
 * @param[in]   dst:目的地址	src:源地址	len:长度
 * @param[out]  None
 * @retval  	None
 */
void ZkMemCpy(void *dst,void *src,const unsigned long len)
{
	char *lD = (char*)dst;
	char *lS = (char*)src;
	unsigned long lTemp = len;
	while(lTemp--){
		*lD++ = *lS++;
	}
}

