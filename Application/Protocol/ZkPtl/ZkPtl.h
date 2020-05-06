/*****************************************************************************
	Copyright (C), 2019-, SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: ZkPtl.h
	Author: Zhang zhiwei Version: V0 Date: 15. July. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _ZKPTL_H_
#define _ZKPTL_H_

//* Public Macros/Configuration and Data type --------------------------------
//* @{公共宏/配置和数据定义区域
#pragma pack(push)	
#pragma pack(1)

//*TAG = 0x00的数据标签*/
/*查询标签*/
typedef struct _bw_tag00_{
	unsigned char Type;	/*查询标签类型 0-App程序标签	1-Boot程序标签*/
}s_BWTAG00_T;
//*TAG = 0x01的数据标签*/
/*查询的标签响应*/
typedef struct _bw_tag01_{
	unsigned char ProductModel[22];	//标签
	unsigned char Version[9];//版本
	unsigned char BuildDate[12];//编译日期
	unsigned char BuildTime[9];//编译时间
	unsigned char Copyright[12];//版权
}s_BWTAG01_T;

//*TAG = 0x02的数据标签*/
/*配置区域操作*/
typedef struct _bw_tag02_{
	unsigned char BlockIndex;//块索引范围0-63
	unsigned char BlockCmd;//块命令
	/*
		1-更新4K配置信息，即索引范围0-63
		2-读取4K配置信息，即索引范围0-63
		3-擦除4K配置信息
	*/
	unsigned char CmdData[64];//命令数据
	/*
		仅命令为1时有效
	*/
}s_BWTAG02_T;
//*TAG = 0x03的数据标签*/
/*配置区域操作结果*/
typedef struct _bw_tag03_{
	unsigned char BlockIndex;//块索引
	unsigned char BlockCmdSta;//块命令操作状态
	/*
		1-更新4K配置信息成功，即索引范围0-63
		2-读取4K配置信息成功，即索引范围0-63
		3-擦除4K配置信息成功
		0xFD-更新4K配置信息索引范围越界
		0xFE-读取配置信息的索引范围越界
		0xFF-未知配置操作命令
	*/
	unsigned char BlockData[64];//块数据
	/*
		仅命令为2时有效
	*/
}s_BWTAG03_T;

//*TAG = 0x04的数据标签*/
/*指令请求*/
typedef struct _bw_tag04_{
	unsigned char Request;
	/*
		1-请求进入boot模式状态
		2-请求进入开始升级状态
		3-请求接收升级数据状态
		4-请求进入结束升级状态
		5-请求进入重启状态
		6-请求读取APP程序的数据块
		7-请求读取BOOT程序的数据块
	*/
	unsigned char ReqData[68];
	/*
		状态2：ReqData[0-3]为APP程序的大小		ReqData[4-7]为APP程序的校验值
		状态3：ReqData[0-63]为APP程序的数据块	ReqData[64-67]为APP程序数据块的索引(范围0-319)
		状态6：ReqData[0-3]为读取APP程序数据块的索引(范围0-319)
		状态7：ReqData[0-3]为读取BOOT程序数据块的索引(范围0-127)
	*/
}s_BWTAG04_T;
//*TAG = 0x05的数据标签*/
/*指令请求结果*/
typedef struct _bw_tag05_{
	unsigned char Respond;
	/*
		1-进入boot模式成功
		2-进入开始升级状态成功
		3-进入升级数据接收成功
		4-进入升级结束状态成功
		5-进入重启状态成功
		6-读取APP程序数据块成功
		7-读取BOOT程序数据块成功
		0xF9-发送读取的BOOT程序数据块索引范围越界
		0xFA-异常重发读取相同的BOOT程序块数据		
		0xFB-发送读取的APP程序数据块索引范围越界
		0xFC-异常重发读取相同的APP程序块数据
		0xFD-发送的程序数据块索引范围越界
		0xFE-异常重发相同的程序数据块
		0xFF-未知指令请求
	*/
	unsigned char ResData[68];
	/*
		状态6：ReqData[0-63]为APP程序的数据块	ReqData[64-67]为APP程序数据块的索引
		状态7：ReqData[0-63]为BOOT程序的数据块	ReqData[64-67]为BOOT程序数据块的索引	
	*/
}s_BWTAG05_T;

//标签类型枚举
typedef enum _tag_type_{
	TAG00_TAG_QUERY = 0,/*标签信息查询*/
	TAG01_TAG_RESPOND,	/*标签信息响应*/
	TAG02_CONFIG_OPT,	/*配置信息操作*/
	TAG03_CONFIG_RES,	/*配置信息操作结果*/
	TAG04_CMD_REQUEST,	/*命令请求*/
	TAG05_CMD_RES,		/*命令请求结果*/

	TAG_UNKNOW,	/* 未知ID信息 */
}e_TAGTYPE_T;

//命令类型
typedef enum _cmd_type_{
	CMD_NONE = 0,			/*0--空命令*/
	CMD_BOOTMODE_IN,		/*1--进入BOOT模式*/
	CMD_UPGRADE_START,		/*2--开始升级*/
	CMD_UPGRADEDATA_RECV,	/*3--接收升级数据*/
	CMD_UPGRADE_END,		/*4--结束升级*/
	CMD_REBOOT,				/*5--重启*/
	CMD_APPDATA_READ,		/*6--APP程序数据读取*/
	CMD_BOOTDATA_READ		/*7--BOOT程序数据读取*/
}e_CMDTYPE_T;
//命令类型响应
typedef enum _cmd_res_{
	CMD_UNKNOW = 0,				/*0--未知命令*/
	CMD_BOOTMODE_SUCCESS,		/*1--进入BOOT模式成功*/
	CMD_UPGRADESTART_SUCCESS,	/*2--开始升级成功*/
	CMD_UPGRADEDATA_SUCCESS,	/*3--接收升级数据成功*/
	CMD_UPGRADEEND_SUCCESS,		/*4--结束升级成功*/
	CMD_REBOOT_SUCCESS,			/*5--重启成功*/
	CMD_APPDATA_SUCCESS,		/*6--APP程序数据读取成功*/
	CMD_BOOTDATA_SUCCESS		/*7--BOOT程序数据读取成功*/
}e_CMDRES_T;

//-----------------------------------------------BST BVT8/9Ptl-----------------------------------------------
#define BST_DISP_ID	0x00
#define BST_BTN_ID	0x05
typedef struct _disp_frame_{
	unsigned char FloorH;	//百位楼层
	unsigned char FloorT;	//十位楼层
	unsigned char FloorU;	//个位楼层
	unsigned char SpecCode;	//特殊显示码，例如箭头
	unsigned char FuncCode[3];//功能码
}s_DISPFRAME_T;
typedef struct _btn_frame_{
	unsigned char Btn;	//按钮信息
	unsigned char Reserve[6];
}s_BTNFRAME_T;
#pragma pack(pop)
//**
// * @}	

//* Public function declaration -----------------------------------------------
//* @{公共函数声明区域(可供外部文件使用)

void ZkPtlInit(void);
void ZkPtlSetBstFrame(unsigned char id,void *dat);
void ZkPtlSetFrameData(e_TAGTYPE_T id,void *dat);
void ZkPtlCopyFrameData(e_TAGTYPE_T id,void *dat);
e_TAGTYPE_T ZkPtlMonitor(void);

//**
// * @}	

#endif //_ZKPTL_H_ 
