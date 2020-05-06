/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: AppConfig.h
	Author: ZhangZhiwei Version: V0 Date: 2019-07-30
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _APPCONFIG_H_
#define _APPCONFIG_H_
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
void AppConfigHandle(void); 
void FixConfigModify(unsigned int magic,unsigned int appsize,unsigned int checksum);
//**
// * @}
#endif //_APPCONFIG_H_
