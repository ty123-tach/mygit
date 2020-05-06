/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: main.c
	Author: Zhang zhiwei Version: V0 Date: 24. June. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "Boot.h"
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
/*!
 * main主函数
 * main主函数
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
int main(void)
{
	HardwareInit();//硬件初始化
	SoftwareInit();//软件初始化
	while(1)
	{		
		BootHandle();
	}
}
