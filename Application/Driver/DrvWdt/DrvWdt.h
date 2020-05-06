/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvWdt.h
	Author: ZhangZhiwei Version: V0 Date: 2019-08-14
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _DRVWDT_H_
#define _DRVWDT_H_
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
void DrvWdtInit(unsigned char en);
void DrvWdtFeed(void);
//**
// * @}
#endif //_DRVWDT_H_
