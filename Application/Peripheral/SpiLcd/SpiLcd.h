/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: SpiLcd.h
	Author: ZhangZhwei Version: V0 Date: 2019-09-02
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _SPILCD_H_
#define _SPILCD_H_
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
void SpiLcdInit(void); 
void SpiLcdHandle(void);
void SpiLcdSleep(void);
void SetSpiLcdRamSeg(unsigned char pos,unsigned char status);
void OtisRslLcdFloorShowSeg(unsigned char pos,unsigned char SegL,unsigned char SegH);
//**
// * @}
#endif //_SPILCD_H_
