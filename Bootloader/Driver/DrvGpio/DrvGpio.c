/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: DrvGpio.c
	Author: Zhang zhiwei Version: V0 Date: 04. July. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "DrvGpio.h"
#include "MKE16Z4.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)

//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)

//**
// * @}
/*!
 * DrvGpioSetPinMux
 * set gpio port function mux
 * 设置GPIO端口复用功能
 * @param[in]   port：端口组
				pin：端口组对应的引脚
				func：复用的功能
 * @param[out]  None
 * @retval  	None
 */
void DrvGpioSetPinMux(void *port,unsigned char pin,unsigned char func)
{
	PORT_Type *base = (PORT_Type*)port;
	base->PCR[pin] = (base->PCR[pin]&(~PORT_PCR_MUX_MASK))|PORT_PCR_MUX(func);
}
