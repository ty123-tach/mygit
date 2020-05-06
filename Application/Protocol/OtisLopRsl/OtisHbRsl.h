/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: OtisHbRsl.h
	Author: ZhiweiZhang Version: V0 Date: 2019-10-24
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _OTISHBRSL_H_
#define _OTISHBRSL_H_
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
void OtisHbRslInit(void);
unsigned char GetHbPortReceiveData(void);
unsigned char GetHbPortTimeOutSta(void);
void SetHbPortSendData(unsigned char data);
void SetHbPortAddress(unsigned char adr);
void SetHbLockSendData(unsigned char data);
void SetHbLockAddress(unsigned char adr);
//**
// * @}
#endif //_OTISHBRSL_H_
