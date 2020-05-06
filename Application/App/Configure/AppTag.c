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
//* @{引用头文件声明区域(仅限本文件内使用)
#include "Configure.h"
#include "absacc.h"
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
/*定义s_TAGINFO_t类型的变量存放在ADR_BOOT_TAG_START地址处*/ 
const s_TAGINFO_t AppSectionMsg __at(CFG_FLASH_APP_TAGADR) =
{
	{"ZK.App"},
	{"Ver1.0.4"},
	{__DATE__},
	{__TIME__},
	{"ZK"},
};

