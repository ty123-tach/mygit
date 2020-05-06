/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: DrvGpio.h
	Author: Zhang zhiwei Version: V0 Date: 04. July. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#ifndef _DRVGPIO_H_
#define _DRVGPIO_H_
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏

/*12号引脚功能*/
#define PIN12_TSIO_CH9		0U
#define PIN12_GPIO_PTB5		1U
#define PIN12_FTM0_CH5		2U
#define PIN12_LPSPI0_PCS1	3U
#define PIN12_TRGUX_IN0		6U
/*16号引脚功能*/
#define PIN16_TSIO_CH10		0U
#define PIN16_GPIO_PTD7		1U
#define PIN16_LPUART2_TX	2U
/*17号引脚功能*/
#define PIN17_TSIO_CH7		0U
#define PIN17_GPIO_PTD6		1U
#define PIN17_LPUART2_RX	2U
/*36号引脚功能*/
#define PIN36_TSIO_CH16		0U
#define PIN36_GPIO_PTC7		1U
#define PIN36_LPUART1_TX	2U
#define PIN36_CAN0_TX		5U
/*37号引脚功能*/
#define PIN37_TSIO_CH15		0U
#define PIN37_GPIO_PTC6		1U
#define PIN37_LPUART1_RX	2U
#define PIN37_CAN0_RX		5U
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void DrvGpioSetPinMux(void *port,unsigned char pin,unsigned char func);
//**
// * @}
#endif
