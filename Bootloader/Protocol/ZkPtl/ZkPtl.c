/*****************************************************************************
	Copyright (C), 2019-, SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: ZkPtl.c
	Author: Zhang zhiwei Version: V0 Date: 15. July. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "ZkPtl.h"
#include "ZkPtlLowlevel.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#pragma pack(push)	
#pragma pack(1)

static unsigned char *ptZkFrame;/*帧指针*/
#pragma pack(pop)
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
unsigned long ZkPtlGetFrameDatLen(e_TAGTYPE_T id);
/*!
 * ZkPtlInit
 * 智控多媒体显示器通讯协议层初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlInit(void)
{
	ZkPtlLowlevelInit();
	ptZkFrame = ZK_EMPTY_FRAME;
}
/*!
 * ZkPtlCopyFrameData
 * 拷贝帧数据
 * @param[in]   id：帧ID	dat：帧数据地址
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlCopyFrameData(e_TAGTYPE_T id,void *dat)
{
	unsigned long FrameLen = 0;
	
	if(ptZkFrame == ZK_EMPTY_FRAME)/*空帧*/
		return;
	if(id >= TAG_UNKNOW)/*未知ID*/
		return;
	FrameLen = ZkPtlGetFrameDatLen(id);
	ZkMemCpy(dat,&ptZkFrame[ZK_FRAMEDAT_POS],FrameLen);
}
/*!
 * ZkPtlSetFrameData
 * 设置帧数据
 * @param[in]   id：帧ID	dat：帧数据地址
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlSetFrameData(e_TAGTYPE_T id,void *dat)
{
	unsigned char DatLen = 0;

	if(id >= TAG_UNKNOW)/*未知ID*/
		return;
	DatLen = ZkPtlGetFrameDatLen(id);
	ZkPtlLowlevelSendData(dat,id,DatLen);
}
/*!
 * ZkPtlMonitor
 * 智控多媒体通讯协议监控
 * @param[in]   None
 * @param[out]  None
 * @retval  	帧ID
 */
e_TAGTYPE_T ZkPtlMonitor(void)
{		
	ptZkFrame = ZkPtlGetLowlevelFrame();
	if(ptZkFrame == ZK_EMPTY_FRAME)
		return TAG_UNKNOW;
	if(ptZkFrame[ZK_FRAMETAG_POS] >= TAG_UNKNOW)
		return TAG_UNKNOW;
	return (e_TAGTYPE_T)ptZkFrame[ZK_FRAMETAG_POS];//返回ID号
}
/*!
 * ZkPtlGetFrameDatLen
 * 获取帧数据长度
 * @param[in]   id：帧ID	
 * @param[out]  None
 * @retval  	帧ID对应的数据长度
 */
unsigned long ZkPtlGetFrameDatLen(e_TAGTYPE_T id)
{
	unsigned long len = 0;
	switch(id)
	{
		case TAG00_TAG_QUERY: len = sizeof(s_BWTAG00_T);break;
		case TAG01_TAG_RESPOND: len = sizeof(s_BWTAG01_T);break;
		case TAG02_CONFIG_OPT: len = sizeof(s_BWTAG02_T);break;
		case TAG03_CONFIG_RES: len = sizeof(s_BWTAG03_T);break;
		case TAG04_CMD_REQUEST: len = sizeof(s_BWTAG04_T);break;
		case TAG05_CMD_RES: len = sizeof(s_BWTAG05_T);break;	
		default:
			break;
	}
	return len;
}

