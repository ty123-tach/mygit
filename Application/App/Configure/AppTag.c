/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: AppTag.c
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "Configure.h"
#include "absacc.h"
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
/*����s_TAGINFO_t���͵ı��������ADR_BOOT_TAG_START��ַ��*/ 
const s_TAGINFO_t AppSectionMsg __at(CFG_FLASH_APP_TAGADR) =
{
	{"ZK.App"},
	{"Ver1.0.4"},
	{__DATE__},
	{__TIME__},
	{"ZK"},
};

