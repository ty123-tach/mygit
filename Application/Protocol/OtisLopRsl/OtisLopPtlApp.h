/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: OtisLopPtlApp.h
	Author: ZhiweiZhang Version: V0 Date: 2019-10-24
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _OTISLOPPTLAPP_H_
#define _OTISLOPPTLAPP_H_
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{���к�����ݶ�������
#define NO_SUPORT_LCD_SHOW	0 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{���к�����������(���ⲿ����ʹ��)
void OtisLopPtlAppInit(void); 
void OtisLopPtlAppHandle(void);
unsigned char OtisRslGetLiftStatus(void);
#if NO_SUPORT_LCD_SHOW
unsigned long OtisRslGetFloorSeg(void);
#endif
//**
// * @}
#endif //_OTISLOPPTLAPP_H_
