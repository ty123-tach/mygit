/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: CfgTagInfo.h
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _CFGTAGINFO_H_
#define _CFGTAGINFO_H_
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{���к�����ݶ�������
/*
	s_TAGINFO_t�ṹ������������������������Ϣ��ǩ
	
	��Ʒ�ͺţ�ProductModel
		TAG_PM_LENTH��������������Ʒ�ͺ���Ϣ���ַ�������
		
	����汾��Version
		TAG_SV_LENTH����������������汾��Ϣ���ַ�������
		
	�������ڣ�BuildDate
		TAG_DA_LENTH����������������������Ϣ���ַ�������
		
	����ʱ�䣺BuildTime
		TAG_TI_LENTH����������������ʱ����Ϣ���ַ�������
		
	��Ȩ��Ϣ��Copyright
		TAG_CR_LENTH��������������Ȩ��Ϣ���ַ�������
*/
#define TAG_PM_LENTH 22
#define TAG_SV_LENTH 9
#define TAG_DA_LENTH 12
#define TAG_TI_LENTH 9
#define TAG_CR_LENTH 12
typedef struct{
	char ProductModel[TAG_PM_LENTH];
	char Version[TAG_SV_LENTH];
	char BuildDate[TAG_DA_LENTH];
	char BuildTime[TAG_TI_LENTH];
	char Copyright[TAG_CR_LENTH];
}s_TAGINFO_t;

/*
	s_CFGMAGIC_T�ṹ��������������APP�������Ϣ
	
	ħ��ֵ��Magic
		��Magicֵ��0x5A5A5A5A�����ʾ������Ҫ������bootģʽ
		
	У��ֵ��CheckSum
		��������Ӧ�ó����У��ֵ
		
	�����С��AppSize
		��������Ӧ�ó���Ĵ�С
*/
#define CFG_MAGIC_VALUE	0x5A5A5A5A
typedef struct{
	unsigned int Magic;
	unsigned int CheckSum;
	unsigned int AppSize;
}s_CFGMAGIC_T;

//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{���к�����������(���ⲿ����ʹ��)
 
//**
// * @}
#endif //_CFGTAGINFO_H_
