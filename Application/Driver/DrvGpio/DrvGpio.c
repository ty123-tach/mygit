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
#include "System.h"
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
				phy_pin_func：芯片实际使用的物理引脚编号功能
 * @param[out]  None
 * @retval  	None
 */
void DrvGpioSetPinMux(void *port,unsigned char pin,unsigned char phy_pin_func)
{
	PORT_Type *base = (PORT_Type*)port;
	if(base == PORTA){
		PeripheralClockEnable(PCC_PORTA_INDEX);
	}else if(base == PORTB){
		PeripheralClockEnable(PCC_PORTB_INDEX);
	}else if(base == PORTC){
		PeripheralClockEnable(PCC_PORTC_INDEX);
	}else if(base == PORTD){
		PeripheralClockEnable(PCC_PORTD_INDEX);
	}else if(base == PORTE){
		PeripheralClockEnable(PCC_PORTE_INDEX);
	}
	base->PCR[pin] = (base->PCR[pin]&(~PORT_PCR_MUX_MASK))|PORT_PCR_MUX(phy_pin_func);
}
/*!
 * DrvGpioOutputLogicLevel
 * gpio pin output logic level
 * GPIO引脚输出逻辑电平
 * @param[in]   gpio：端口组
				pin：端口组对应的引脚
				level：逻辑电平 0-低电平	非0-高电平
 * @param[out]  None
 * @retval  	None
 */
void DrvGpioOutputLogicLevel(void *gpio,unsigned char pin,unsigned char level)
{
	GPIO_Type *base = (GPIO_Type*)gpio;
	if(level){
		base->PSOR = 1<<pin;
	}else{
		base->PCOR = 1<<pin;		
	}

}
/*!
 * DrvGpioSetDirection
 * 设置GPIO的电平方向
 * @param[in]   gpio：使用的GPIO组
				pin：GPIO组对应的引脚编号
				dir：0-GPIO为输入方向	非0-GPIO为输出方向
 * @param[out]  None
 * @retval  	None
 */
void DrvGpioSetDirection(void *gpio,unsigned char pin,unsigned char dir)
{
	GPIO_Type *base = (GPIO_Type*)gpio;
	if(dir){
		base->PDDR |= (1<<pin);	
	}else{
		base->PDDR &= ~(1<<pin);	
		if(base == GPIOA){
			PORTA->PCR[pin] |= 0x03;//Pull enable and pull up
		}else if(base == GPIOB){
			PORTB->PCR[pin] |= 0x03;
		}else if(base == GPIOC){
			PORTC->PCR[pin] |= 0x03;
		}else if(base == GPIOD){
			PORTD->PCR[pin] |= 0x03;
		}else if(base == GPIOE){	
			PORTE->PCR[pin] |= 0x03;
		}	
	}
}
/*!
 * DrvGpioSetInterrupt
 * 设置GPIO的中断
 * @param[in]   port：使用的GPIO端口
				pin：GPIO组对应的引脚编号
				type：中断类型
 * @param[out]  None
 * @retval  	None
 */
void DrvGpioSetInterrupt(void *port,unsigned char pin,unsigned char type)
{
	PORT_Type *base = (PORT_Type*)port;
	base->DFWR |= PORT_DFWR_FILT(31);
	base->PCR[pin] =(base->PCR[pin] & ~PORT_PCR_IRQC_MASK)|PORT_PCR_IRQC(type)|PORT_PCR_PFE(1);
	base->DFER |= PORT_DFER_DFE(pin);
}
