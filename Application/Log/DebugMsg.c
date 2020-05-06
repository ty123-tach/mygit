/*****************************************************************************
	Copyright (C), 2010-2011, XC Electric Co., Ltd.
	File name: DebugMsg.c
	Author: Zhang zhiwei Version: V0 Date: 6. March. 2015
	Description:  ��ӡ�������ģ��
	Version: V0
	Function List: 
        1-McuPrintInit()    ��ӡ������ڳ�ʼ��
        2-fputc()           ��ӡ����ض���
	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "DebugMsg.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)

//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)

//**
// * @}
#if DEBUG_INFO_OUT
#pragma import(__use_no_semihosting_swi)


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

LOGSEN DebugLogSenCh;/*���ͺ���*/
LOGINIT DebugLogInit;/*��ʼ��*/
/*******************************************************************************
	Function:			DebugRegister
	Description:	    ע�������Ϣ�ⲿӲ���ӿ�
	Input:				SendCh��Ӳ�����ڷ���һ���ֽں���
						GetCh��Ӳ�����ڽ���һ���ֽں���
						ComInit��Ӳ�����ڳ�ʼ��
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
	Description:	    printf����ض������һ���ַ�
	Input:				ch ��������ַ� f���ļ�ָ��
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
	Description:	    scanf�����ض���
	Input:				f���ļ�ָ��
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
	Description:	    ���ø����������������
	Input:				f���ļ�ָ��
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
	Description:	    ������
	Input:				ch��һ���ַ�
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
	Description:	    ϵͳ�˳�
	Input:				return_code���˳�ǰ�ķ���ֵ
	Output:				None
	Return:				None
	Others:				None
********************************************************************************/
void _sys_exit(int return_code) 
{
	while (1);    /* endless loop */
}
#endif
