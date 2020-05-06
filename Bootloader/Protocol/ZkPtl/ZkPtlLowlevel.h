/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: ZkPtlLowlevel.h
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _ZKPTLLOWLEVEL_H_
#define _ZKPTLLOWLEVEL_H_
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{公有宏和数据定义区域
#define ZK_ESC 0x1B	/*转换字符*/
#define ZK_STX 0x80	/*帧起始字符*/
#define ZK_ETX 0x81	/*帧结束字符*/
#define ZK_STX_AF 0xE7	/* 帧起始转换后增加的字符 */
#define ZK_ETX_AF 0xE8	/* 帧结束转换后增加的字符 */
#define ZK_ESC_AF 0x00	/* 转换字符转换后增加的字符 */  
#define ZK_FRAMETAG_POS	0x01	/*帧标签位置*/
#define ZK_FRAMEDAT_POS	0x02	/*帧数据位置*/
#define ZK_EMPTY_FRAME	0		/*空帧*/
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{公有函数声明区域(供外部调用使用)
void ZkPtlLowlevelInit(void);
void ZkPtlLowlevelRecCh(unsigned char ch);
void ZkPtlLowlevelSendData(void *dat,unsigned char tag,unsigned long len);
unsigned char *ZkPtlGetLowlevelFrame(void);
void ZkMemCpy(void *dst,void *src,const unsigned long len);
//**
// * @}
#endif //_ZKPTLLOWLEVEL_H_
