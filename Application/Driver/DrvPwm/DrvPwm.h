/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvPwm.h
	Author: ZhangZhiwei Version: V0 Date: 2019-08-13
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _DRVPWM_H_
#define _DRVPWM_H_
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{���к�����ݶ�������
 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{���к�����������(���ⲿ����ʹ��)
void DrvPwmInt(void);
void DrvSetPwmValue(unsigned char ch,unsigned int value);
//**
// * @}
#endif //_DRVPWM_H_
