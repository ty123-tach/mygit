/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: SpiLcd.h
	Author: ZhangZhwei Version: V0 Date: 2019-09-02
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _SPILCD_H_
#define _SPILCD_H_
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{公有宏和数据定义区域
 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{公有函数声明区域(供外部调用使用)
void SpiLcdInit(void); 
void SpiLcdHandle(void);
void SpiLcdSleep(void);
void SetSpiLcdRamSeg(unsigned char pos,unsigned char status);
void OtisRslLcdFloorShowSeg(unsigned char pos,unsigned char SegL,unsigned char SegH);
//**
// * @}
#endif //_SPILCD_H_
