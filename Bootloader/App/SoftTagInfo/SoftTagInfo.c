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
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "SoftTagInfo.h"
#include "Configure.h"
#include "ZkPtl.h"
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
 
/*!
 * SoftTagInfoHandle
 * software tag information handle
 * �����ǩ��Ϣ����
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
