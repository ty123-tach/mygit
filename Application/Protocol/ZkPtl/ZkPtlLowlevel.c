/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: ZkPtlLowlevel.c
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "ZkPtlLowlevel.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define MAX_LOWRECV_BUFSIZE	2	/*���ĵײ����ݽ��ջ�������С*/
#define MAX_PTLDATA_LENTH	144	/*һ��Э���������Ŀ��ܳ���*/
#define SRC_SENDBUF_INDEX	0
#define DST_SENDBUF_INDEX	1
static unsigned char gLowSendBuf[2][MAX_PTLDATA_LENTH];
static unsigned char gLowRecvBuf[MAX_LOWRECV_BUFSIZE][MAX_PTLDATA_LENTH];
static unsigned char gLowRecvLen[MAX_LOWRECV_BUFSIZE];
static unsigned char gLowUseBuf;/*��ǰ����ʹ�õĻ�����*/
static unsigned char gLowUsePos;/*��ǰ����ʹ�õĻ���������λ��*/
static unsigned char gLowHandleBuf;/*��ǰ������Ļ�����*/
#define STA_IS_UNKNOW 0		/*δ֪״̬*/
#define STA_IS_RECEIVE	1	/*���ڽ���״̬*/
static unsigned char gLowRecvStatus;//����״̬
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
unsigned long ZkPtlRecoverFrame(unsigned char *src,unsigned long srclen);
unsigned long ZkPtlTransformFrame(void *src,unsigned long srclen,void *dst);
unsigned char ZkPtltVerify(unsigned char *data, unsigned long length);
extern void ZkPtlHwSendData(void *dat,unsigned long size);
//**
// * @}
/*!
 * ZkPtlLowlevelInit
 * �ײ��ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlLowlevelInit(void)
{
	gLowRecvStatus = STA_IS_UNKNOW;
	gLowHandleBuf = 0;
	gLowUseBuf = 0;
	gLowUsePos = 0;
}
/*!
 * ZkPtlLowlevelRecCh
 * �ײ����ݽ���
 * @param[in]   ch�����յ��ֽ�����
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlLowlevelRecCh(unsigned char ch)
{
	if(ch == ZK_STX){//���յ���ʼ�ַ�
		gLowRecvStatus = STA_IS_RECEIVE;
		gLowUsePos = 0;
		gLowRecvBuf[gLowUseBuf][gLowUsePos++] = ch;
	}else if(ch == ZK_ETX){//���յ������ַ�
		gLowRecvBuf[gLowUseBuf][gLowUsePos++] = ch;
		gLowRecvLen[gLowUseBuf] = gLowUsePos;
		/*�л����ջ�����*/
		gLowUseBuf++;
		if(gLowUseBuf >= MAX_LOWRECV_BUFSIZE)
			gLowUseBuf = 0;
		gLowRecvStatus = STA_IS_UNKNOW;
	}else{
		if(gLowRecvStatus != STA_IS_RECEIVE)//�ǽ���״̬
			return;
		if(gLowUsePos < MAX_PTLDATA_LENTH)//������δ��
			gLowRecvBuf[gLowUseBuf][gLowUsePos++] = ch;
	}
}
/*!
 * ZkPtlLowlevelSendData
 * �ײ㷢������
 * @param[in]   dat�������͵�����
				len�������͵����ݳ���
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlLowlevelSendData(void *dat,unsigned char tag,unsigned long len)
{
	unsigned long TranLen = 0;/*ת����֡����*/
	unsigned char *ptSend = gLowSendBuf[SRC_SENDBUF_INDEX];

	*ptSend++ = ZK_STX;	/*����֡ͷ*/
	*ptSend++ = tag;	/*�����ǩ*/
	ZkMemCpy(ptSend,dat,len);/*��������*/
	ptSend += len;
	*ptSend++ = ZkPtltVerify(&gLowSendBuf[SRC_SENDBUF_INDEX][1],len+1);/*����У��*/
	*ptSend++ = ZK_ETX;/*����֡β*/
	TranLen = ZkPtlTransformFrame(gLowSendBuf[SRC_SENDBUF_INDEX],
					ptSend-gLowSendBuf[SRC_SENDBUF_INDEX],gLowSendBuf[DST_SENDBUF_INDEX]);/*֡ת��*/
	ZkPtlHwSendData(gLowSendBuf[DST_SENDBUF_INDEX],TranLen);
}
/*!
 * ZkPtlLowlevelSendBstData
 * �ײ㷢��BSTЭ������
 * @param[in]   dat�������͵�����
				tag�������͵����ݱ�ǩ
				len�������͵����ݳ���
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlLowlevelSendBstData(void *dat,unsigned char tag,unsigned char len)
{
	unsigned char *ptSend = gLowSendBuf[SRC_SENDBUF_INDEX];
	
	*ptSend++ = ZK_STX;	/*����֡ͷ*/
	*ptSend++ = tag;	/*�����ǩ*/
	ZkMemCpy(ptSend,dat,len);/*��������*/
	ptSend += len;
	*ptSend++ = ZkPtltVerify(&gLowSendBuf[SRC_SENDBUF_INDEX][1],len+1);/*����У��*/
	*ptSend++ = ZK_ETX;/*����֡β*/	
	ZkPtlHwSendData(gLowSendBuf[SRC_SENDBUF_INDEX],len+4);
}
/*!
 * ZkPtlGetLowlevelFrame
 * �ײ��ȡ֡����
 * @param[in]   None
 * @param[out]  None
 * @retval  	֡���ݵ�ַ
 */
unsigned char *ZkPtlGetLowlevelFrame(void)
{
	unsigned long TranFrameLen = 0;	/*ת���󳤶�*/
	unsigned char temp;
	unsigned char *Frame;
	
	if(gLowHandleBuf == gLowUseBuf)//��֡����
		return ZK_EMPTY_FRAME;
	temp = gLowHandleBuf;
	TranFrameLen = ZkPtlRecoverFrame(gLowRecvBuf[temp],gLowRecvLen[temp]);/*֡ת��*/
	if(gLowRecvBuf[temp][TranFrameLen-2] != 
		ZkPtltVerify(&gLowRecvBuf[temp][1],TranFrameLen-3)){/* ����У��ʧ�� */	
		Frame = ZK_EMPTY_FRAME;
	}else{
		Frame = gLowRecvBuf[temp];
	}
	gLowHandleBuf++;
	if(gLowHandleBuf >= MAX_LOWRECV_BUFSIZE)
		gLowHandleBuf = 0;
	return Frame;
}
/*!
 * ZkPtlRecoverFrame
 * Э��ȥ���ַ�ת�����ָ�����
 * @param[in]   src��ԭ���ݵ�ַ len��ԭ���ݳ���
 * @param[out]  None
 * @retval  	�ָ����֡����
 */
unsigned long ZkPtlRecoverFrame(unsigned char *src,unsigned long srclen)
{
	unsigned long Len = srclen-2;/*ȥ��֡ͷ��֡β*/
	unsigned long TranLen = 2;/*�ָ��󳤶�,֡ͷ��֡β*/
	unsigned long Cnt = 0;
	unsigned char *lDst = src;
	unsigned char *lSrc = src;
	
	*lDst++ = *lSrc++;/*ȡ��֡ͷ����*/
	while(Cnt < Len)
	{
		if(*lSrc == ZK_ESC)
		{
			if(*(lSrc+1) == ZK_STX_AF)//0xE7
				*lDst++ = ZK_STX;
			else if(*(lSrc+1) == ZK_ETX_AF)//0xE8
				*lDst++ = ZK_ETX;
			else if(*(lSrc+1) == ZK_ESC_AF)//0x00
				*lDst++ = ZK_ESC;
			lSrc += 2;
			Cnt += 2;
		}else{
			*lDst++ = *lSrc++;
			Cnt++;
		}
		TranLen++;
	}
	*lDst = *lSrc;/*ȡ��֡β����*/
	return TranLen;
}
/*!
 * ZkPtlTransformFrame
 * Э��ת��֡����
 * @param[in]   src��ԭ���ݵ�ַ dst��ת�������ݵ�ַ 
				len��ԭ���ݳ���
 * @param[out]  None
 * @retval  	����ת����ĳ���
 */
unsigned long ZkPtlTransformFrame(void *src,unsigned long srclen,void *dst)
{
	unsigned long Len = srclen-2;/*ȥ��֡ͷ��֡β*/
	unsigned long TranLen = 2;
	unsigned char Cnt = 0;
	unsigned char *lDst = (unsigned char*)dst;
	unsigned char *lSrc = (unsigned char*)src;
	
	*lDst++ = *lSrc++;/*ȡ��֡ͷ����*/
	while(Cnt < Len)
	{
		if(*lSrc == ZK_STX)/*����֡ͷ����*/
		{
			*lDst++ = ZK_ESC;
			*lDst++ = ZK_STX_AF;
			TranLen += 2;
		}else if(*lSrc == ZK_ETX){/*����֡β����*/
			*lDst++ = ZK_ESC;
			*lDst++ = ZK_ETX_AF;
			TranLen += 2;
		}else if(*lSrc == ZK_ESC){/*����ת���ַ�*/
			*lDst++ = ZK_ESC;
			*lDst++ = ZK_ESC_AF;
			TranLen += 2;
		}else{
			*lDst++ = *lSrc;
			TranLen++;
		}
		lSrc++;
		Cnt++;	
	}	
	*lDst = *lSrc;/*ȡ��֡β����*/
	return TranLen;
}
/*!
 * ZkPtltVerify
 * Э��У���㷨
 * @param[in]   data����У�������ָ��	length����У������ݳ���
 * @param[out]  None
 * @retval  	None
 */
unsigned char ZkPtltVerify(unsigned char *data, unsigned long length)
{
	unsigned char result = 0;
	unsigned int i;
	
	for (i = 0; i < length; i++){
		result = result ^ data[i];
	}	
	return result & 0x7f;
}
/*!
 * ZkMemCpy
 * �ڴ濽��
 * @param[in]   dst:Ŀ�ĵ�ַ	src:Դ��ַ	len:����
 * @param[out]  None
 * @retval  	None
 */
void ZkMemCpy(void *dst,void *src,const unsigned long len)
{
	char *lD = (char*)dst;
	char *lS = (char*)src;
	unsigned long lTemp = len;
	while(lTemp--){
		*lD++ = *lS++;
	}
}

