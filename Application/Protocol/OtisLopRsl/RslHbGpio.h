/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: RslHbGpio.h
	Author: ZhangZhwei Version: V0 Date: 2019-09-02
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _RSLHBGPIO_H_
#define _RSLHBGPIO_H_
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{公有宏和数据定义区域
#define RSL_UPBTN_KEY		(0x0040U)	/*RSL上按钮键值*/
#define RSL_DNBTN_KEY		(0x0080U)	/*RSL下按钮键值*/
#define RSL_LOCKBTN_KEY		(0x0100U)	/*RSL锁梯按钮键值*/
#define RSL_LIGHT_ON		1			/*RSL按钮亮灯*/
#define RSL_LIGHT_OFF		0			/*RSL按钮灭灯*/
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{公有函数声明区域(供外部调用使用)
void RslHbGpioInit(void); 
void RslHbGpioInputHandle(void);
void SetHbDnButtonStatus(unsigned char sta);
void SetHbUpButtonStatus(unsigned char sta);
unsigned char GetHbGpioDevAdr(void);
unsigned char GetHbGpioKeyStatus(unsigned short key);
unsigned char GetHbButtonValue(void);
//**
// * @}
#endif //_RSLHBGPIO_H_
