/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvPwm.h
	Author: ZhangZhiwei Version: V0 Date: 2019-08-13
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _DRVPWM_H_
#define _DRVPWM_H_
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
void DrvPwmInt(void);
void DrvSetPwmValue(unsigned char ch,unsigned int value);
//**
// * @}
#endif //_DRVPWM_H_
