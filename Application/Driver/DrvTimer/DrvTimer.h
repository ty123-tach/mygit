/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: DrvTimer.h
	Author: Zhang zhiwei Version: V0 Date: 02. July. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#ifndef _DRVTIMER_H_
#define _DRVTIMER_H_
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)

//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
unsigned int DrvGetTimerMsTick(void);
void DrvTickDelayMs(unsigned int ms);
void DrvTimerInit(void);
void DrvTimerDeinit(void);
//**
// * @}
#endif
