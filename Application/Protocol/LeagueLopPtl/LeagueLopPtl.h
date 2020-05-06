/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: LeagueLopPtl.h
	Author: ZhiweiZhang Version: V0 Date: 2019-12-29
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _LEAGUELOPPTL_H_
#define _LEAGUELOPPTL_H_
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
void LeagueLopPtlInit(void);
void LeagueLopPtlHandle(void);
void setLeagueLopAdr(unsigned char type,unsigned char adr);
void setLeagueLopButton(unsigned char btn);
void setLeagueLopFireBtn(unsigned char sta);
void setLeagueLopLockBtn(unsigned char sta);
void setLeagueLopUpBtn(unsigned char sta);
void setLeagueLopDnBtn(unsigned char sta);
unsigned char getLeagueLopButtonLight(void);
unsigned char getLeagueLopWorkMode(void);
unsigned char getLeagueLopFloorAdr(void);
//**
// * @}
#endif //_LEAGUELOPPTL_H_
