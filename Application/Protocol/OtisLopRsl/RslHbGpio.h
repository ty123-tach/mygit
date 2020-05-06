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
//* @{����ͷ�ļ���������
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{���к�����ݶ�������
#define RSL_UPBTN_KEY		(0x0040U)	/*RSL�ϰ�ť��ֵ*/
#define RSL_DNBTN_KEY		(0x0080U)	/*RSL�°�ť��ֵ*/
#define RSL_LOCKBTN_KEY		(0x0100U)	/*RSL���ݰ�ť��ֵ*/
#define RSL_LIGHT_ON		1			/*RSL��ť����*/
#define RSL_LIGHT_OFF		0			/*RSL��ť���*/
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{���к�����������(���ⲿ����ʹ��)
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
