/*****************************************************************************
	Copyright (C), 2010-2011, XC Electric Co., Ltd.
	File name: DebugMsg.c
	Author: Zhang zhiwei Version: V0 Date: 6. March. 2015
	Description:  打印输出功能模块
	Version: V0
	Function List: 
        1-McuPrintInit()    打印输出串口初始化
        2-fputc()           打印输出重定向
	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "DebugMsg.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)

//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)

//**
// * @}
#if DEBUG_INFO_OUT
#pragma import(__use_no_semihosting_swi)


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

LOGSEN DebugLogSenCh;/*发送函数*/
LOGINIT DebugLogInit;/*初始化*/
/*******************************************************************************
	Function:			DebugRegister
	Description:	    注册调试信息外部硬件接口
	Input:				SendCh：硬件串口发送一个字节函数
						GetCh：硬件串口接收一个字节函数
						ComInit：硬件串口初始化
	Output:				None
	Return:				None
	Others:				None
********************************************************************************/
void DebugRegister(LOGSEN SendCh,LOGINIT ComInit)
{
	DebugLogSenCh = SendCh;
	DebugLogInit = ComInit;
	DebugLogInit();
}
/*******************************************************************************
	Function:			fputc
	Description:	    printf输出重定向输出一个字符
	Input:				ch 待输出的字符 f：文件指针
	Output:				None
	Return:				None
	Others:				None
********************************************************************************/
int fputc(int ch, FILE *f) 
{
	DebugLogSenCh(ch);
	return (ch);
}
/*******************************************************************************
	Function:			fgetc
	Description:	    scanf输入重定向
	Input:				f：文件指针
	Output:				None
	Return:				None
	Others:				None
********************************************************************************/
int fgetc(FILE *f) 
{
	return 0;
}
/*******************************************************************************
	Function:			ferror
	Description:	    调用各种输入输出出错报告
	Input:				f：文件指针
	Output:				None
	Return:				None
	Others:				None
********************************************************************************/
int ferror(FILE *f)    
{
	/* Your implementation of ferror */
	return EOF;
}
/*******************************************************************************
	Function:			_ttywrch
	Description:	    错误处理
	Input:				ch：一个字符
	Output:				None
	Return:				None
	Others:				None
********************************************************************************/
void _ttywrch(int ch) 
{
	DebugLogSenCh(ch);
}
/*******************************************************************************
	Function:			_sys_exit
	Description:	    系统退出
	Input:				return_code：退出前的返回值
	Output:				None
	Return:				None
	Others:				None
********************************************************************************/
void _sys_exit(int return_code) 
{
	while (1);    /* endless loop */
}
#endif
