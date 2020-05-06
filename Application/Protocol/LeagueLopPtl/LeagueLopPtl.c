/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: LeagueLopPtl.c
	Author: ZhiweiZhang Version: V0 Date: 2019-12-29
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "LeagueLopPtl.h"
#include "SegLedLopCtrl.h"
#include "DrvCan.h"
#include "DrvTimer.h"
#include "DebugMsg.h"
#include "Configure.h"
#include "DrvFlash.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)

#define LOP_WORK_CFG_ADR	0x00002400	//LOP������Ϣ��ַ

#define CAN_LOPDISP_ID			0x01		//LOP��ʾ֡ID
#define CAN_FDUPTBN_LIGHT_ID	0x525064	//LOPǰ���ϰ�ť��ID
#define CAN_FDDNBTN_LIGHT_ID	0x525065	//LOPǰ���°�ť��ID
#define CAN_BDUPTBN_LIGHT_ID	0x525066	//LOP�����ϰ�ť��ID
#define CAN_BDDNBTN_LIGHT_ID	0x525067	//LOP�����°�ť��ID
#define CAN_MASTER_SCAN_ID		0x525068	//���ƹ�ɨ��ID
#define CAN_BASE_FDBTN_ID		0x5250C8	//ǰ�������ťID����ַ��Ӧ��ID=0x5250C8+����¥��
#define CAN_BASE_BDBTN_ID		0x525109	//���������ťID����ַ��Ӧ��ID=0x525109+����¥��
#define CAN_SCAN_RESPOND_ID		0x52514A	//Ӧ����ƹ�ɨ��ID


/*COP CAN������Ϣ*/
static s_USRDATA_T gLopCanCfg; 
static s_CANFRAME_T gRecvFrame;
static s_CANFRAME_T gDispFrame;		//��ʾ֡����
static s_CANFRAME_T gFdUpBtnFrame;	//ǰ���ϰ�ť֡����
static s_CANFRAME_T gFdDnBtnFrame;	//ǰ���°�ť֡����
static s_CANFRAME_T gBdUpBtnFrame;	//�����ϰ�ť֡����
static s_CANFRAME_T gBdDnBtnFrame;	//�����°�ť֡����
static s_CANFRAME_T gScanFrame;		//���ƹ�ɨ�������֡����

#define FRAME_DISP_SHIFT	(0x01U)
#define FRAME_DISP_MASK		(0xFEU)

#define FRAME_FDUPBTN_SHIFT	(0x02U)
#define FRAME_FDUPBTN_MASK	(0xFDU)

#define FRAME_FDDNBTN_SHIFT	(0x04U)
#define FRAME_FDDNBTN_MASK	(0xFBU)

#define FRAME_BDUPBTN_SHIFT	(0x08U)
#define FRAME_BDUPBTN_MASK	(0xF7U)

#define FRAME_BDDNBTN_SHIFT	(0x10U)
#define FRAME_BDDNBTN_MASK	(0xEFU)

#define FRAME_SCAN_SHIFT	(0x20U)
#define FRAME_SCAN_MASK		(0xDFU)

static unsigned char gRecvFrameFlag;
static unsigned char gLopType;		//LOP���� 0-ǰ�� 1-����
static unsigned char gLopFloorAdr;	//LOP��ַ ��Χ1-64
static unsigned char gLopButtonSta;	//��ť״̬
static unsigned char gLopButtonLight;//��ť��״̬
static unsigned char gLopButtonChange;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void LeagueLopCanIrqHandle(void);
void DisplayFrameHandle(void);
void FrontDoorUpBtnLightHandle(void);
void FrontDoorDnBtnLightHandle(void);
void BackDoorUpBtnLightHandle(void);
void BackDoorDnBtnLightHandle(void);
void ScanFrameHandle(void);
void LeagueLopReportButtonStatus(void);
unsigned char HexToAscii(unsigned char value);
//**
// * @}
/*!
 * getLeagueLopWorkMode
 * ��ȡLOP����ģʽ
 * @param[in]   None
 * @param[out]  None
 * @retval  	1��ǰ��	2������
 */
unsigned char getLeagueLopWorkMode(void)
{
	return gLopType;
}
/*!
 * getLeagueLopFloorAdr
 * ��ȡLOP¥���ַ
 * @param[in]   None
 * @param[out]  None
 * @retval  	¥���ַ
 */
unsigned char getLeagueLopFloorAdr(void)
{
	return gLopFloorAdr;
}
/*!
 * getLeagueLopButtonLight
 * ��ȡ��ť��״̬
 * @param[in]   None
 * @param[out]  None
 * @retval  	��״̬
 */
unsigned char getLeagueLopButtonLight(void)
{
	return gLopButtonLight;
}
/*!
 * setLeagueLopButton
 * ����LOP��ť
 * @param[in]   btn����ť״̬
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopButton(unsigned char btn)
{
	if(btn == gLopButtonSta)
		return;
	gLopButtonSta = btn;
	gLopButtonChange = 1;
}
/*!
 * setLeagueLopFireBtn
 * ����������ť״̬
 * @param[in]   sta����0������	0���ɿ�
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopFireBtn(unsigned char sta)
{
	if(sta){
		if((gLopButtonSta&0x04) == 0){
			gLopButtonSta |= 0x04;
			gLopButtonChange = 1;
		}
	}else{
		if(gLopButtonSta&0x04){
			gLopButtonSta &= 0xFB;
			gLopButtonChange = 1;
		}
	}
}
/*!
 * setLeagueLopLockBtn
 * �������ݰ�ť״̬
 * @param[in]   sta����0������	0���ɿ�
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopLockBtn(unsigned char sta)
{
	if(sta){
		if((gLopButtonSta&0x08) == 0){
			gLopButtonSta |= 0x08;
			gLopButtonChange = 1;
		}
	}else{
		if(gLopButtonSta&0x08){
			gLopButtonSta &= 0xF7;
			gLopButtonChange = 1;
		}
	}
}
/*!
 * setLeagueLopUpBtn
 * �������а�ť״̬
 * @param[in]   sta����0������	0���ɿ�
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopUpBtn(unsigned char sta)
{
	if(sta){
		if((gLopButtonSta&0x01) == 0){
			gLopButtonSta |= 0x01;
			gLopButtonChange = 1;
		}
	}else{
		if(gLopButtonSta&0x01){
			gLopButtonSta &= 0xFE;
			gLopButtonChange = 1;
		}
	}
}
/*!
 * setLeagueLopUpBtn
 * �������а�ť״̬
 * @param[in]   sta����0������	0���ɿ�
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopDnBtn(unsigned char sta)
{
	if(sta){
		if((gLopButtonSta&0x02) == 0){
			gLopButtonSta |= 0x02;
			gLopButtonChange = 1;
		}
	}else{
		if(gLopButtonSta&0x02){
			gLopButtonSta &= 0xFD;
			gLopButtonChange = 1;
		}
	}
}
/*!
 * setLeagueLopAdr
 * ����LOP��ַ
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void setLeagueLopAdr(unsigned char type,unsigned char adr)
{
	unsigned int lLopCfg;
	if((type == 0)||(type > 2)||(adr == 0)||(adr > 64))
		return;
	if((gLopType == type)&&(adr == gLopFloorAdr))
		return;
	gLopType = type;
	gLopFloorAdr = adr;
	lLopCfg = gLopFloorAdr;
	lLopCfg <<= 8;
	lLopCfg |= gLopType;
	DrvFlashEraseSector(CFG_FLASH_APPCFG_INDEXSTART+1);
	DrvFlashWriteLongWord(LOP_WORK_CFG_ADR,lLopCfg);
}
/*!
 * LeagueLopPtlInit
 * ����LOPЭ���ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueLopPtlInit(void)
{
	unsigned int lCopCfg;
	gLopCanCfg.recvId1 = CAN_LOPDISP_ID|EXT_CANFRAME_TYPE;;
	gLopCanCfg.recvId1Mask = 0x00;
	gLopCanCfg.recvId2 = CAN_FDUPTBN_LIGHT_ID|EXT_CANFRAME_TYPE;
	gLopCanCfg.recvId2Mask = 0x0F;
	gLopCanCfg.recvError = 0;
	gLopCanCfg.sendError = 0;
	gLopCanCfg.ptFrame = &gRecvFrame;
	gLopCanCfg.recvIRQ = LeagueLopCanIrqHandle;
	DrvCanInit(40000,&gLopCanCfg); 
	gRecvFrameFlag = 0;
	gLopButtonChange = 0;
	gLopButtonSta = 0;
	
	lCopCfg = *((unsigned int*)LOP_WORK_CFG_ADR);
	if(lCopCfg == 0xFFFFFFFF){
		lCopCfg = 0x0101;
		setLeagueLopAdr(1,1);
	}
	gLopType = lCopCfg&0xFF;
	gLopFloorAdr = (lCopCfg>>8)&0xFF;
	
	if(gLopType == 0)
		gLopType = 1;
	else if(gLopType >2)
		gLopType = 1;
	
	if(gLopFloorAdr == 0)
		gLopFloorAdr = 1;
	else if(gLopFloorAdr > 64)
		gLopFloorAdr = 64;
}
/*!
 * LeagueLopPtlHandle
 * ����LOPЭ�鴦��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueLopPtlHandle(void)
{
	DisplayFrameHandle();
	if(gLopType == 1){//ǰ��
		FrontDoorUpBtnLightHandle();
		FrontDoorDnBtnLightHandle();
	}else if(gLopType == 2){//����
		BackDoorUpBtnLightHandle();
		BackDoorDnBtnLightHandle();
	}
	ScanFrameHandle();
	LeagueLopReportButtonStatus();
}
/*!
 * FrontDoorUpBtnLightHandle
 * ǰ���ϰ�ť�ƴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void FrontDoorUpBtnLightHandle(void)
{
	unsigned char lBitPos;
	unsigned char lBufPos;
	if((gRecvFrameFlag&FRAME_FDUPBTN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_FDUPBTN_MASK;
	lBitPos = 1<<((gLopFloorAdr-1)%8);
	lBufPos = (gLopFloorAdr-1)/8;
	if(lBufPos > 7)
		return;
	if(gFdUpBtnFrame.Data[lBufPos]&lBitPos){
		//�ϰ�ť����
		gLopButtonLight |= 0x01;
	}else{
		gLopButtonLight &= 0xFE;
		//�ϰ�ť����
	}
}
/*!
 * FrontDoorDnBtnLightHandle
 * ǰ���°�ť�ƴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void FrontDoorDnBtnLightHandle(void)
{
	unsigned char lBitPos;
	unsigned char lBufPos;	
	if((gRecvFrameFlag&FRAME_FDDNBTN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_FDDNBTN_MASK;
	lBitPos = 1<<((gLopFloorAdr-1)%8);
	lBufPos = (gLopFloorAdr-1)/8;
	if(lBufPos > 7)
		return;
	if(gFdDnBtnFrame.Data[lBufPos]&lBitPos){
		//�°�ť����
		gLopButtonLight |= 0x02;
	}else{
		//�°�ť����
		gLopButtonLight &= 0xFD;
	}
}
/*!
 * BackDoorUpBtnLightHandle
 * �����ϰ�ť�ƴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BackDoorUpBtnLightHandle(void)
{
	unsigned char lBitPos;
	unsigned char lBufPos;		
	if((gRecvFrameFlag&FRAME_BDUPBTN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_BDUPBTN_MASK;
	lBitPos = 1<<((gLopFloorAdr-1)%8);
	lBufPos = (gLopFloorAdr-1)/8;
	if(lBufPos > 7)
		return;
	if(gBdUpBtnFrame.Data[lBufPos]&lBitPos){
		//�ϰ�ť����
		gLopButtonLight |= 0x01;
	}else{
		gLopButtonLight &= 0xFE;
		//�ϰ�ť����
	}
}
/*!
 * BackDoorDnBtnLightHandle
 * �����°�ť�ƴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BackDoorDnBtnLightHandle(void)
{
	unsigned char lBitPos;
	unsigned char lBufPos;	
	if((gRecvFrameFlag&FRAME_BDDNBTN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_BDDNBTN_MASK;
	lBitPos = 1<<((gLopFloorAdr-1)%8);
	lBufPos = (gLopFloorAdr-1)/8;
	if(lBufPos > 7)
		return;
	if(gBdDnBtnFrame.Data[lBufPos]&lBitPos){
		//�°�ť����
		gLopButtonLight |= 0x02;
	}else{
		//�°�ť����
		gLopButtonLight &= 0xFE;
	}
}
/*!
 * DisplayFrameHandle
 * ��ʾ֡����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DisplayFrameHandle(void)
{
	unsigned char lValue;
	if((gRecvFrameFlag&FRAME_DISP_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_DISP_MASK;
	setSegLedShowArrow(gDispFrame.Data[1]&0x03);
	setSegLedShowOutOfService(gDispFrame.Data[1]&0x08);
	setSegLedShowFullload(gDispFrame.Data[5]&0x04);
	if(gDispFrame.Data[2]&0x80){//���շ�ʽ1��ʾ3λ¥��
		lValue = gDispFrame.Data[2] - 0x80;
		setSegLedShowThreeFloor(HexToAscii((lValue>>4)&0x0F),
								HexToAscii(lValue&0x0F),gDispFrame.Data[3]);
	}else if(gDispFrame.Data[3]&0x80){//���շ�ʽ2��ʾ3λ¥��
		lValue = gDispFrame.Data[3] - 0x80;
		setSegLedShowThreeFloor(gDispFrame.Data[2],HexToAscii((lValue>>4)&0x0F),
								HexToAscii(lValue&0x0F));	
	}else{//��λ¥����ʾ
		setSegLedShowTwoFloor(gDispFrame.Data[2],gDispFrame.Data[3]);
	}
}
/*!
 * HexToAscii
 * 0-F HexֵתASCII��
 * @param[in]   Hexֵ
 * @param[out]  None
 * @retval  	None
 */
unsigned char HexToAscii(unsigned char value)
{
	if(value < 10){
		return '0'+value;
	}else if(value < 16){
		return 'A'+(value-10);
	}
	return 0;
}
/*!
 * ScanFrameHandle
 * ���ƹ�ɨ��֡Ӧ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ScanFrameHandle(void)
{
	s_CANFRAME_T lScanFrame;
	if((gRecvFrameFlag&FRAME_SCAN_SHIFT) == 0)
		return;
	gRecvFrameFlag &= FRAME_SCAN_MASK;
	if(gScanFrame.Data[1] != gLopType)//�ǵ�ǰLOP����
		return;
	if(gScanFrame.Data[0] != gLopFloorAdr)//¥��ֵ��ƥ��
		return;	
	lScanFrame.Id = CAN_SCAN_RESPOND_ID;
	lScanFrame.Flag = 0x08| EXT_CANFRAME_TYPE;	
	lScanFrame.Data[0] = gScanFrame.Data[0];
	lScanFrame.Data[1] = gScanFrame.Data[1];
	lScanFrame.Data[2] = gScanFrame.Data[2];
	lScanFrame.Data[3] = *((unsigned char*)(CFG_FLASH_APP_TAGADR+TAG_PM_LENTH+3));
	lScanFrame.Data[4] = *((unsigned char*)(CFG_FLASH_APP_TAGADR+TAG_PM_LENTH+5));;
	lScanFrame.Data[5] = 0;
	lScanFrame.Data[6] = 0;
	lScanFrame.Data[7] = 0;
	DrvCanSendFrame(&lScanFrame);
}
/*!
 * LeagueLopReportButtonStatus
 * �ϱ���ť״̬
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueLopReportButtonStatus(void)
{
	s_CANFRAME_T lBtnFrame;
	if(gLopButtonChange == 0)
		return;
	gLopButtonChange = 0;
	if(gLopType == 1){//ǰ��
		lBtnFrame.Id = CAN_BASE_FDBTN_ID+gLopFloorAdr;
	}else{
		lBtnFrame.Id = CAN_BASE_BDBTN_ID+gLopFloorAdr;
	}
	lBtnFrame.Flag = 0x08| EXT_CANFRAME_TYPE;
	lBtnFrame.Data[0] = gLopButtonSta;
	lBtnFrame.Data[1] = 0;
	lBtnFrame.Data[2] = 0;
	lBtnFrame.Data[3] = 0;
	lBtnFrame.Data[4] = 0;
	lBtnFrame.Data[5] = 0;
	lBtnFrame.Data[6] = 0;
	lBtnFrame.Data[7] = 0;
	DrvCanSendFrame(&lBtnFrame);
}
/*!
 * LeagueLopCanIrqHandle
 * CAN�жϴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueLopCanIrqHandle(void)
{
//	if((gRecvFrame.Flag&EXT_CANFRAME_TYPE) == 0)//����չ֡
//		return;
	if(gRecvFrame.Id == CAN_LOPDISP_ID){
		gDispFrame = gRecvFrame;
		gRecvFrameFlag |= FRAME_DISP_SHIFT;
	}else if(gRecvFrame.Id == CAN_MASTER_SCAN_ID){
		gScanFrame = gRecvFrame;
		gRecvFrameFlag |= FRAME_SCAN_SHIFT;
	}else{
		if(gLopType == 1){//ǰ��
			if(gRecvFrame.Id == CAN_FDUPTBN_LIGHT_ID){
				gFdUpBtnFrame = gRecvFrame;
				gRecvFrameFlag |= FRAME_FDUPBTN_SHIFT;
			}else if(gRecvFrame.Id == CAN_FDDNBTN_LIGHT_ID){
				gFdDnBtnFrame = gRecvFrame;
				gRecvFrameFlag |= FRAME_FDDNBTN_SHIFT;
			}	
		}else if(gLopType == 2){//����
			if(gRecvFrame.Id == CAN_BDUPTBN_LIGHT_ID){
				gBdUpBtnFrame = gRecvFrame;
				gRecvFrameFlag |= FRAME_BDUPBTN_SHIFT;
			}else if(gRecvFrame.Id == CAN_BDDNBTN_LIGHT_ID){
				gBdDnBtnFrame = gRecvFrame;
				gRecvFrameFlag |= FRAME_BDDNBTN_SHIFT;
			}	
		}
	}
}
