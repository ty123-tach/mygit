/*******************************************************************************
	Copyright (C), 2010-2011, XC Electric Co., Ltd.
	File name: DebugMsg.h
	Author: zhang zhiwei Version: V0 Date: 6. March. 2015
	Description: 
      调试信息打印输出初始化和重定向
	Others: 
	Function List: 
        1-DebugMsgINIT() 信息打印输出口初始化
        2-DebugMsg()     信息打印输出
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
//* @{公共头文件
#include "stdio.h"
//#include "SEGGER_RTT.h"
//**
// * @}	*/
//* Public Macros/Configuration and Data type --------------------------------
//* @{公共宏/配置和数据定义区域(源文件中可供外部文件使用的宏/配置和数据)
/*------------------------------------------------------------/
/ Debug Message Configurations 调试信息打印输出配置
/------------------------------------------------------------*/
#define DEBUG_INFO_OUT 1	
/*
  当DEBUG_INFO_OUT配置为1时,
  
  DebugMsg()功能可用来打印调试信息,
  函数参数格式与标准C语言的printf格式相同。
  
  GetDebugInfo()功能可以用来获取串口输入的调试信息
  函数参数格式与标准C语言的scanf()格式相同
  
  当DEBUG_INFO_OUT
  配置为0时,DebugMsg()、GetDebugInfo()功能禁用。默认禁用
*/
//**
// * @}	

//* Public function declaration -----------------------------------------------
//* @{公共函数声明区域(可供外部文件使用)
#if DEBUG_INFO_OUT
	/*定义串口一个字节输出函数指针*/
	typedef void (*LOGSEN)(unsigned char ch);
	/*定义串口初始化函数指针*/
	typedef void (*LOGINIT)(void);
	
	/*注册调试信息外部硬件接口*/
	void DebugRegister(LOGSEN SendCh,LOGINIT ComInit);
	/*调试信息打印函数*/
	#define DebugMsg(args...)	printf(args)
	/*获取调试信息函数*/
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
