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
//* @{引用头文件声明区域
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{公有宏和数据定义区域

#define CFG_FLASH_SECTORSIZE	0x00000400	//FLASH的扇区大小1K	单位：字节
/*32K Flash地址分区*/
#define CFG_FLASH_BOOT_CODEADR	0x00000000	//BOO程序地址
#define CFG_FLASH_BOOT_TAGADR	0x00000600	//BOOT标签地址
#define CFG_FLASH_APP_CFGADR	0x00002000	//APP配置区地址
#define CFG_FLASH_APP_CODEADR	0x00003000	//APP程序地址
#define CFG_FLASH_APP_TAGADR	0x00003600	//APP标签地址

/*Flash分区扇区大小明细*/
#define CFG_FLASH_APPCFG_SECTORNUM		4	//APP配置区占用扇区数量
#define CFG_FLASH_APPCODE_SECTORNUM		20	//APP代码占用扇区数量
#define CFG_FLASH_TOTAL_SECTORNUM		32	//总的FLASH扇区数量

/*Flash分区占用大小明细*/
#define CFG_FLASH_APPCFG_SIZE	0x1000	//APP配置区域空间大小
#define CFG_FLASH_APPCODE_SIZE	0x5000	//APP代码区域空间大小

/*Flash扇区编号索引明细*/
#define CFG_FLASH_BOOT_INDEXSTART		0	//BOOT索引起始
#define CFG_FLASH_BOOT_INDEXEN			7	//BOOT索引结束
#define CFG_FLASH_APPCFG_INDEXSTART		8	//APP配置索引起始
#define CFG_FLASH_APPCFG_INDEXEN		11	//APP配置索引结束
#define CFG_FLASH_APPCODE_INDEXSTART	12	//APP代码索引起始
#define CFG_FLASH_APPCODE_INDEXEN		31	//APP代码索引结束

//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{公有函数声明区域(供外部调用使用)
 
//**
// * @}
#endif //_CFGFLASHPARTITION_H_
