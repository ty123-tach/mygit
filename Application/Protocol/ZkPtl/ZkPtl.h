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
//* @{������/���ú����ݶ�������
#pragma pack(push)	
#pragma pack(1)

//*TAG = 0x00�����ݱ�ǩ*/
/*��ѯ��ǩ*/
typedef struct _bw_tag00_{
	unsigned char Type;	/*��ѯ��ǩ���� 0-App�����ǩ	1-Boot�����ǩ*/
}s_BWTAG00_T;
//*TAG = 0x01�����ݱ�ǩ*/
/*��ѯ�ı�ǩ��Ӧ*/
typedef struct _bw_tag01_{
	unsigned char ProductModel[22];	//��ǩ
	unsigned char Version[9];//�汾
	unsigned char BuildDate[12];//��������
	unsigned char BuildTime[9];//����ʱ��
	unsigned char Copyright[12];//��Ȩ
}s_BWTAG01_T;

//*TAG = 0x02�����ݱ�ǩ*/
/*�����������*/
typedef struct _bw_tag02_{
	unsigned char BlockIndex;//��������Χ0-63
	unsigned char BlockCmd;//������
	/*
		1-����4K������Ϣ����������Χ0-63
		2-��ȡ4K������Ϣ����������Χ0-63
		3-����4K������Ϣ
	*/
	unsigned char CmdData[64];//��������
	/*
		������Ϊ1ʱ��Ч
	*/
}s_BWTAG02_T;
//*TAG = 0x03�����ݱ�ǩ*/
/*��������������*/
typedef struct _bw_tag03_{
	unsigned char BlockIndex;//������
	unsigned char BlockCmdSta;//���������״̬
	/*
		1-����4K������Ϣ�ɹ�����������Χ0-63
		2-��ȡ4K������Ϣ�ɹ�����������Χ0-63
		3-����4K������Ϣ�ɹ�
		0xFD-����4K������Ϣ������ΧԽ��
		0xFE-��ȡ������Ϣ��������ΧԽ��
		0xFF-δ֪���ò�������
	*/
	unsigned char BlockData[64];//������
	/*
		������Ϊ2ʱ��Ч
	*/
}s_BWTAG03_T;

//*TAG = 0x04�����ݱ�ǩ*/
/*ָ������*/
typedef struct _bw_tag04_{
	unsigned char Request;
	/*
		1-�������bootģʽ״̬
		2-������뿪ʼ����״̬
		3-���������������״̬
		4-��������������״̬
		5-�����������״̬
		6-�����ȡAPP��������ݿ�
		7-�����ȡBOOT��������ݿ�
	*/
	unsigned char ReqData[68];
	/*
		״̬2��ReqData[0-3]ΪAPP����Ĵ�С		ReqData[4-7]ΪAPP�����У��ֵ
		״̬3��ReqData[0-63]ΪAPP��������ݿ�	ReqData[64-67]ΪAPP�������ݿ������(��Χ0-319)
		״̬6��ReqData[0-3]Ϊ��ȡAPP�������ݿ������(��Χ0-319)
		״̬7��ReqData[0-3]Ϊ��ȡBOOT�������ݿ������(��Χ0-127)
	*/
}s_BWTAG04_T;
//*TAG = 0x05�����ݱ�ǩ*/
/*ָ��������*/
typedef struct _bw_tag05_{
	unsigned char Respond;
	/*
		1-����bootģʽ�ɹ�
		2-���뿪ʼ����״̬�ɹ�
		3-�����������ݽ��ճɹ�
		4-������������״̬�ɹ�
		5-��������״̬�ɹ�
		6-��ȡAPP�������ݿ�ɹ�
		7-��ȡBOOT�������ݿ�ɹ�
		0xF9-���Ͷ�ȡ��BOOT�������ݿ�������ΧԽ��
		0xFA-�쳣�ط���ȡ��ͬ��BOOT���������		
		0xFB-���Ͷ�ȡ��APP�������ݿ�������ΧԽ��
		0xFC-�쳣�ط���ȡ��ͬ��APP���������
		0xFD-���͵ĳ������ݿ�������ΧԽ��
		0xFE-�쳣�ط���ͬ�ĳ������ݿ�
		0xFF-δָ֪������
	*/
	unsigned char ResData[68];
	/*
		״̬6��ReqData[0-63]ΪAPP��������ݿ�	ReqData[64-67]ΪAPP�������ݿ������
		״̬7��ReqData[0-63]ΪBOOT��������ݿ�	ReqData[64-67]ΪBOOT�������ݿ������	
	*/
}s_BWTAG05_T;

//��ǩ����ö��
typedef enum _tag_type_{
	TAG00_TAG_QUERY = 0,/*��ǩ��Ϣ��ѯ*/
	TAG01_TAG_RESPOND,	/*��ǩ��Ϣ��Ӧ*/
	TAG02_CONFIG_OPT,	/*������Ϣ����*/
	TAG03_CONFIG_RES,	/*������Ϣ�������*/
	TAG04_CMD_REQUEST,	/*��������*/
	TAG05_CMD_RES,		/*����������*/

	TAG_UNKNOW,	/* δ֪ID��Ϣ */
}e_TAGTYPE_T;

//��������
typedef enum _cmd_type_{
	CMD_NONE = 0,			/*0--������*/
	CMD_BOOTMODE_IN,		/*1--����BOOTģʽ*/
	CMD_UPGRADE_START,		/*2--��ʼ����*/
	CMD_UPGRADEDATA_RECV,	/*3--������������*/
	CMD_UPGRADE_END,		/*4--��������*/
	CMD_REBOOT,				/*5--����*/
	CMD_APPDATA_READ,		/*6--APP�������ݶ�ȡ*/
	CMD_BOOTDATA_READ		/*7--BOOT�������ݶ�ȡ*/
}e_CMDTYPE_T;
//����������Ӧ
typedef enum _cmd_res_{
	CMD_UNKNOW = 0,				/*0--δ֪����*/
	CMD_BOOTMODE_SUCCESS,		/*1--����BOOTģʽ�ɹ�*/
	CMD_UPGRADESTART_SUCCESS,	/*2--��ʼ�����ɹ�*/
	CMD_UPGRADEDATA_SUCCESS,	/*3--�����������ݳɹ�*/
	CMD_UPGRADEEND_SUCCESS,		/*4--���������ɹ�*/
	CMD_REBOOT_SUCCESS,			/*5--�����ɹ�*/
	CMD_APPDATA_SUCCESS,		/*6--APP�������ݶ�ȡ�ɹ�*/
	CMD_BOOTDATA_SUCCESS		/*7--BOOT�������ݶ�ȡ�ɹ�*/
}e_CMDRES_T;

//-----------------------------------------------BST BVT8/9Ptl-----------------------------------------------
#define BST_DISP_ID	0x00
#define BST_BTN_ID	0x05
typedef struct _disp_frame_{
	unsigned char FloorH;	//��λ¥��
	unsigned char FloorT;	//ʮλ¥��
	unsigned char FloorU;	//��λ¥��
	unsigned char SpecCode;	//������ʾ�룬�����ͷ
	unsigned char FuncCode[3];//������
}s_DISPFRAME_T;
typedef struct _btn_frame_{
	unsigned char Btn;	//��ť��Ϣ
	unsigned char Reserve[6];
}s_BTNFRAME_T;
#pragma pack(pop)
//**
// * @}	

//* Public function declaration -----------------------------------------------
//* @{����������������(�ɹ��ⲿ�ļ�ʹ��)

void ZkPtlInit(void);
void ZkPtlSetBstFrame(unsigned char id,void *dat);
void ZkPtlSetFrameData(e_TAGTYPE_T id,void *dat);
void ZkPtlCopyFrameData(e_TAGTYPE_T id,void *dat);
e_TAGTYPE_T ZkPtlMonitor(void);

//**
// * @}	

#endif //_ZKPTL_H_ 
