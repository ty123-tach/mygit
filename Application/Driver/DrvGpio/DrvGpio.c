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
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "DrvGpio.h"
#include "MKE16Z4.h"
#include "System.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)

//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)

//**
// * @}
/*!
 * DrvGpioSetPinMux
 * set gpio port function mux
 * ����GPIO�˿ڸ��ù���
 * @param[in]   port���˿���
				pin���˿����Ӧ������
				phy_pin_func��оƬʵ��ʹ�õ��������ű�Ź���
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
 * GPIO��������߼���ƽ
 * @param[in]   gpio���˿���
				pin���˿����Ӧ������
				level���߼���ƽ 0-�͵�ƽ	��0-�ߵ�ƽ
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
 * ����GPIO�ĵ�ƽ����
 * @param[in]   gpio��ʹ�õ�GPIO��
				pin��GPIO���Ӧ�����ű��
				dir��0-GPIOΪ���뷽��	��0-GPIOΪ�������
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
 * ����GPIO���ж�
 * @param[in]   port��ʹ�õ�GPIO�˿�
				pin��GPIO���Ӧ�����ű��
				type���ж�����
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
