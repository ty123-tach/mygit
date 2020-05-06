/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: SoftTagInfo.c
	Author: ZhangZhiwei Version: V0 Date: 2019-07-29
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "SoftTagInfo.h"
#include "Configure.h"
#include "ZkPtl.h"
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
 * SoftTagInfoHandle
 * software tag information handle
 * 软件标签信息处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void SoftTagInfoHandle(void)
{
	s_BWTAG00_T Query;
	
	ZkPtlCopyFrameData(TAG00_TAG_QUERY,&Query);
	if(Query.Type == 0){
		ZkPtlSetFrameData(TAG01_TAG_RESPOND,(unsigned char*)CFG_FLASH_APP_TAGADR);
	}else{
		ZkPtlSetFrameData(TAG01_TAG_RESPOND,(unsigned char*)CFG_FLASH_BOOT_TAGADR);
	}
}
