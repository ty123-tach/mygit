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
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "ZkPtl.h"
#include "ZkPtlLowlevel.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#pragma pack(push)	
#pragma pack(1)

static unsigned char *ptZkFrame;/*ָ֡��*/
#pragma pack(pop)
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
unsigned long ZkPtlGetFrameDatLen(e_TAGTYPE_T id);
/*!
 * ZkPtlInit
 * �ǿض�ý����ʾ��ͨѶЭ����ʼ��
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
 * ����֡����
 * @param[in]   id��֡ID	dat��֡���ݵ�ַ
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlCopyFrameData(e_TAGTYPE_T id,void *dat)
{
	unsigned long FrameLen = 0;
	
	if(ptZkFrame == ZK_EMPTY_FRAME)/*��֡*/
		return;
	if(id >= TAG_UNKNOW)/*δ֪ID*/
		return;
	FrameLen = ZkPtlGetFrameDatLen(id);
	ZkMemCpy(dat,&ptZkFrame[ZK_FRAMEDAT_POS],FrameLen);
}
/*!
 * ZkPtlSetFrameData
 * ����֡����
 * @param[in]   id��֡ID	dat��֡���ݵ�ַ
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlSetFrameData(e_TAGTYPE_T id,void *dat)
{
	unsigned char DatLen = 0;

	if(id >= TAG_UNKNOW)/*δ֪ID*/
		return;
	DatLen = ZkPtlGetFrameDatLen(id);
	ZkPtlLowlevelSendData(dat,id,DatLen);
}
/*!
 * ZkPtlMonitor
 * �ǿض�ý��ͨѶЭ����
 * @param[in]   None
 * @param[out]  None
 * @retval  	֡ID
 */
e_TAGTYPE_T ZkPtlMonitor(void)
{		
	ptZkFrame = ZkPtlGetLowlevelFrame();
	if(ptZkFrame == ZK_EMPTY_FRAME)
		return TAG_UNKNOW;
	if(ptZkFrame[ZK_FRAMETAG_POS] >= TAG_UNKNOW)
		return TAG_UNKNOW;
	return (e_TAGTYPE_T)ptZkFrame[ZK_FRAMETAG_POS];//����ID��
}
/*!
 * ZkPtlGetFrameDatLen
 * ��ȡ֡���ݳ���
 * @param[in]   id��֡ID	
 * @param[out]  None
 * @retval  	֡ID��Ӧ�����ݳ���
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

