/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvFlash.h
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _DRVFLASH_H_
#define _DRVFLASH_H_
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
void DrvFlashEraseSector(unsigned char sector);
void DrvFlashWriteLongWord(unsigned int adr,unsigned int data);
void DrvFlashBlock64Write(unsigned int adr,unsigned char *dat);
//**
// * @}
#endif //_DRVFLASH_H_
