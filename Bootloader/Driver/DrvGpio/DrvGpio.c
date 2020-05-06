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
				func�����õĹ���
 * @param[out]  None
 * @retval  	None
 */
void DrvGpioSetPinMux(void *port,unsigned char pin,unsigned char func)
{
	PORT_Type *base = (PORT_Type*)port;
	base->PCR[pin] = (base->PCR[pin]&(~PORT_PCR_MUX_MASK))|PORT_PCR_MUX(func);
}
