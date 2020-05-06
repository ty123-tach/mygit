/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvADC.h
	Author: ZhangZhiwei Version: V0 Date: 2019-08-09
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _DRVADC_H_
#define _DRVADC_H_
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{公有宏和数据定义区域
#define ADCCH0_EXTFUNC_USE	1
#define ADCCH1_EXTFUNC_USE	2
#define ADCCH2_EXTFUNC_USE	3
#define ADCCH3_EXTFUNC_USE	4
#define ADCCH4_EXTFUNC_USE	5
#define ADCCH5_EXTFUNC_USE	6
#define ADCCH6_EXTFUNC_USE	7
#define ADCCH7_EXTFUNC_USE	8
#define ADCCH8_EXTFUNC_USE	9
#define ADCCH9_EXTFUNC_USE	10
#define ADCCH10_EXTFUNC_USE	11
#define ADCCH11_EXTFUNC_USE	12
#define ADCCH12_EXTFUNC_USE	13

typedef void (*pFuncGetAdcValue)(unsigned short value);	//获取ADC采样值，外部函数
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{公有函数声明区域(供外部调用使用)
void DrvAdcInit(void);
void DrvAdcTriggerConversion(unsigned char channel);
unsigned char DrvAdcWorkStatus(void);
//ADC采样值函数注册 1：成功	0：失败
//ch：ADC通道
//extfunc：读取值函数
unsigned char DrvAdcGetValueFuncRegister(unsigned char ch,pFuncGetAdcValue extfunc);
//**
// * @}
#endif //_DRVADC_H_
