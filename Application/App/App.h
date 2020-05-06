/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: App.h
	Author: Zhang zhiwei Version: V0 Date: 24. June. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#ifndef _APP_H_
#define _APP_H_
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)

//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void HardwareInit(void);
void SoftwareInit(void);
void AppHandle(void);
void ZkPtlHandle(void);
void ZkLiftHandle(void);
//**
// * @}
#endif
