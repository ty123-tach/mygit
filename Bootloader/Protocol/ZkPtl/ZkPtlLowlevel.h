/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: ZkPtlLowlevel.h
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _ZKPTLLOWLEVEL_H_
#define _ZKPTLLOWLEVEL_H_
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{���к�����ݶ�������
#define ZK_ESC 0x1B	/*ת���ַ�*/
#define ZK_STX 0x80	/*֡��ʼ�ַ�*/
#define ZK_ETX 0x81	/*֡�����ַ�*/
#define ZK_STX_AF 0xE7	/* ֡��ʼת�������ӵ��ַ� */
#define ZK_ETX_AF 0xE8	/* ֡����ת�������ӵ��ַ� */
#define ZK_ESC_AF 0x00	/* ת���ַ�ת�������ӵ��ַ� */  
#define ZK_FRAMETAG_POS	0x01	/*֡��ǩλ��*/
#define ZK_FRAMEDAT_POS	0x02	/*֡����λ��*/
#define ZK_EMPTY_FRAME	0		/*��֡*/
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{���к�����������(���ⲿ����ʹ��)
void ZkPtlLowlevelInit(void);
void ZkPtlLowlevelRecCh(unsigned char ch);
void ZkPtlLowlevelSendData(void *dat,unsigned char tag,unsigned long len);
unsigned char *ZkPtlGetLowlevelFrame(void);
void ZkMemCpy(void *dst,void *src,const unsigned long len);
//**
// * @}
#endif //_ZKPTLLOWLEVEL_H_
