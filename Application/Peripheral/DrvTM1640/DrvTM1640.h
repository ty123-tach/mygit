/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvTM1640.h
	Author: Li Hongpu Version: V0 Date: 2019-07-17
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _DRVTM1640_H_
#define _DRVTM1640_H_
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{公有宏和数据定义区域
 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{公有函数声明区域(供外部调用使用)
void DrvTM1640Init(void);
void DrvTM1640SendData(void *dat,unsigned char dat_size,unsigned char dat_len);
void DrvTm1640Clear(void);
//**
// * @}
#endif //_DRVTM1640_H_
