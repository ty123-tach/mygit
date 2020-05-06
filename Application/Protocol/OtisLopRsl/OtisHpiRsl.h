/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: OtisHpiRsl.h
	Author: ZhiweiZhang Version: V0 Date: 2019-10-24
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _OTISHPIRSL_H_
#define _OTISHPIRSL_H_
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
void OtisHpiRslInit(void); 
unsigned char *GetHpiReceiveData(void);
unsigned char GetHpiTimeOut(void);
void HpiReceiveBufferClean(void);
//**
// * @}
#endif //_OTISHPIRSL_H_
