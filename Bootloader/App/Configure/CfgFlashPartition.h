/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: CfgFlashPartition.h
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _CFGFLASHPARTITION_H_
#define _CFGFLASHPARTITION_H_
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{���к�����ݶ�������

#define CFG_FLASH_SECTORSIZE	0x00000400	//FLASH��������С1K	��λ���ֽ�
/*32K Flash��ַ����*/
#define CFG_FLASH_BOOT_CODEADR	0x00000000	//BOO�����ַ
#define CFG_FLASH_BOOT_TAGADR	0x00000600	//BOOT��ǩ��ַ
#define CFG_FLASH_APP_CFGADR	0x00002000	//APP��������ַ
#define CFG_FLASH_APP_CODEADR	0x00003000	//APP�����ַ
#define CFG_FLASH_APP_TAGADR	0x00003600	//APP��ǩ��ַ

/*Flash����������С��ϸ*/
#define CFG_FLASH_APPCFG_SECTORNUM		4	//APP������ռ����������
#define CFG_FLASH_APPCODE_SECTORNUM		20	//APP����ռ����������
#define CFG_FLASH_TOTAL_SECTORNUM		32	//�ܵ�FLASH��������

/*Flash����ռ�ô�С��ϸ*/
#define CFG_FLASH_APPCFG_SIZE	0x1000	//APP��������ռ��С
#define CFG_FLASH_APPCODE_SIZE	0x5000	//APP��������ռ��С

/*Flash�������������ϸ*/
#define CFG_FLASH_BOOT_INDEXSTART		0	//BOOT������ʼ
#define CFG_FLASH_BOOT_INDEXEN			7	//BOOT��������
#define CFG_FLASH_APPCFG_INDEXSTART		8	//APP����������ʼ
#define CFG_FLASH_APPCFG_INDEXEN		11	//APP������������
#define CFG_FLASH_APPCODE_INDEXSTART	12	//APP����������ʼ
#define CFG_FLASH_APPCODE_INDEXEN		31	//APP������������

//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{���к�����������(���ⲿ����ʹ��)
 
//**
// * @}
#endif //_CFGFLASHPARTITION_H_
