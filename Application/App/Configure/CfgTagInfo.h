/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: CfgTagInfo.h
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _CFGTAGINFO_H_
#define _CFGTAGINFO_H_
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{公有宏和数据定义区域
/*
	s_TAGINFO_t结构体数据类型描述程序的相关信息标签
	
	产品型号：ProductModel
		TAG_PM_LENTH——用来描述产品型号信息的字符串长度
		
	软件版本：Version
		TAG_SV_LENTH——用来描述软件版本信息的字符串长度
		
	编译日期：BuildDate
		TAG_DA_LENTH——用来描述编译日期信息的字符串长度
		
	编译时间：BuildTime
		TAG_TI_LENTH——用来描述编译时间信息的字符串长度
		
	版权信息：Copyright
		TAG_CR_LENTH——用来描述版权信息的字符串长度
*/
#define TAG_PM_LENTH 22
#define TAG_SV_LENTH 9
#define TAG_DA_LENTH 12
#define TAG_TI_LENTH 9
#define TAG_CR_LENTH 12
typedef struct{
	char ProductModel[TAG_PM_LENTH];
	char Version[TAG_SV_LENTH];
	char BuildDate[TAG_DA_LENTH];
	char BuildTime[TAG_TI_LENTH];
	char Copyright[TAG_CR_LENTH];
}s_TAGINFO_t;

/*
	s_CFGMAGIC_T结构体数据类型描述APP程序的信息
	
	魔术值：Magic
		若Magic值不0x5A5A5A5A，则表示程序需要运行在boot模式
		
	校验值：CheckSum
		用来描述应用程序的校验值
		
	程序大小：AppSize
		用来描述应用程序的大小
*/
#define CFG_MAGIC_VALUE	0x5A5A5A5A
typedef struct{
	unsigned int Magic;
	unsigned int CheckSum;
	unsigned int AppSize;
}s_CFGMAGIC_T;

//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{公有函数声明区域(供外部调用使用)
 
//**
// * @}
#endif //_CFGTAGINFO_H_
