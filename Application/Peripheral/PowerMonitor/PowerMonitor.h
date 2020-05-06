/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: PowerMonitor.h
	Author: ZhangZhiwei Version: V0 Date: 2019-09-02
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _POWERMONITOR_H_
#define _POWERMONITOR_H_
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
void PowerMonitorInit(void); 
void PowerSystemAdcHandle(void);
float GetSystemPower(void);
//**
// * @}
#endif //_POWERMONITOR_H_
