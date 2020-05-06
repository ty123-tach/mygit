/*******************************************************************************
	Copyright (C), 2010-2011, XC Electric Co., Ltd.
	File name: DebugMsg.h
	Author: zhang zhiwei Version: V0 Date: 6. March. 2015
	Description: 
      ������Ϣ��ӡ�����ʼ�����ض���
	Others: 
	Function List: 
        1-DebugMsgINIT() ��Ϣ��ӡ����ڳ�ʼ��
        2-DebugMsg()     ��Ϣ��ӡ���
	History: 
	1.
		Date:
		Author:
		Modification:
	2. 
******************************************************************************/
#ifndef _DEBUGMSG_H_
#define _DEBUGMSG_H_
//* Public include files ------------------------------------------------------
//** @incgroup for xxx.c file use
//* @{����ͷ�ļ�
#include "stdio.h"
//#include "SEGGER_RTT.h"
//**
// * @}	*/
//* Public Macros/Configuration and Data type --------------------------------
//* @{������/���ú����ݶ�������(Դ�ļ��пɹ��ⲿ�ļ�ʹ�õĺ�/���ú�����)
/*------------------------------------------------------------/
/ Debug Message Configurations ������Ϣ��ӡ�������
/------------------------------------------------------------*/
#define DEBUG_INFO_OUT 1	
/*
  ��DEBUG_INFO_OUT����Ϊ1ʱ,
  
  DebugMsg()���ܿ�������ӡ������Ϣ,
  ����������ʽ���׼C���Ե�printf��ʽ��ͬ��
  
  GetDebugInfo()���ܿ���������ȡ��������ĵ�����Ϣ
  ����������ʽ���׼C���Ե�scanf()��ʽ��ͬ
  
  ��DEBUG_INFO_OUT
  ����Ϊ0ʱ,DebugMsg()��GetDebugInfo()���ܽ��á�Ĭ�Ͻ���
*/
//**
// * @}	

//* Public function declaration -----------------------------------------------
//* @{����������������(�ɹ��ⲿ�ļ�ʹ��)
#if DEBUG_INFO_OUT
	/*���崮��һ���ֽ��������ָ��*/
	typedef void (*LOGSEN)(unsigned char ch);
	/*���崮�ڳ�ʼ������ָ��*/
	typedef void (*LOGINIT)(void);
	
	/*ע�������Ϣ�ⲿӲ���ӿ�*/
	void DebugRegister(LOGSEN SendCh,LOGINIT ComInit);
	/*������Ϣ��ӡ����*/
	#define DebugMsg(args...)	printf(args)
	/*��ȡ������Ϣ����*/
	#define GetDebugInfo(args...)	scanf(args)
	#define JlinkDebugMsg(args...) SEGGER_RTT_printf(0,args)
	#define JlinkDebugInit()	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP)
#else 
	#define DebugMsg(args...)
	#define JlinkDebugMsg(args...)
	#define JlinkDebugInit()
	
#endif
//**
// * @}	

#endif //_DEBUGMSG_H_ 
