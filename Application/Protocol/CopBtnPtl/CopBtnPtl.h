/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: CopBtnPtl.h
	Author: ZhiweiZhang Version: V0 Date: 2020-01-07
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _COPBTNPTL_H_
#define _COPBTNPTL_H_
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
void CopBtnPtlInit(void); 
void CopBtnPtlHandle(void);
void BtnPtlRWCmdInit(unsigned char mode);
void BtnPtlReadIdCmd(unsigned char adr);
void BtnPtlAdrSetCmd(unsigned char adr);
void BtnPtlSetLightStatus(unsigned char adr,unsigned char sta);
unsigned char BtnPtlGetKeyStatus(unsigned char adr);
unsigned char *BtnPtlKeyStatusBuf(void);
//**
// * @}
#endif //_COPBTNPTL_H_
