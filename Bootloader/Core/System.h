/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: System.h
	Author: Zhang zhiwei Version: V0 Date: 27. June. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#ifndef _SYSTEM_H_
#define _SYSTEM_H_
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
/*system oscillator bus clock source*/
#define CLK_SOBC_SRC	1U
/*Slow IRC clock*/
#define CLK_SIRC_SRC	2U
/*Fast IRC clock*/
#define CLK_FIRC_SRC	3U
/*Low-power FLL clock*/
#define CLK_LPFLL_SRC	5U
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void PeripheralClockSrcSelect(unsigned int perinum,unsigned char clksrc);
void PeripheralClockEnable(unsigned int perinum);
void SystemSetup(void);
void JumpToApplication(void);
//**
// * @}
#endif
