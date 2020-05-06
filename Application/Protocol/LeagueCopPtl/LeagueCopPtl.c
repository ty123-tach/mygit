/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: LeagueCopPtl.c
	Author: ZhiweiZhang Version: V0 Date: 2019-12-25
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "LeagueCopPtl.h"
#include "DrvCan.h"
#include "SegLedCtrl.h"
#include "BoardLed.h"
#include "DrvTimer.h"
#include "CopBtnPtl.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define LEAGUE_COP_CANID	0x01	//����ID��Ϣ
#define FRONT_DOOR_LIGHTID	0x190	//ǰ�Ű�ť��ָ��
#define BACK_DOOR_LIGHTID	0x191	//���Ű�ť��ָ��
#define FRONT_DOOR_FLASHID	0x192	//ǰ�Ű�ť����˸ָ��
#define BACK_DOOR_FLASHID	0x193	//���Ű�ť����˸ָ��
#define COP_CALL_BTNID		0x700	//��ť����ָ��
/*COP CAN������Ϣ*/
#define ID_BASE_SHIFT	(0x01U)
#define ID_BASE_MASK	(0xFEU)
#define ID_FRONT_DL_SHIFT	(0x02U)
#define ID_FRONT_DL_MASK	(0xFDU)
#define ID_BACK_DL_SHIFT	(0x04U)
#define ID_BACK_DL_MASK		(0xFBU)
static s_USRDATA_T gCopCanCfg;
static s_CANFRAME_T gRecvFrame;
static s_CANFRAME_T gBaseFrame;	//������Ϣָ��֡
static s_CANFRAME_T gFrontDL;	//ǰ�Ű�ť��ָ��֡
static s_CANFRAME_T gBackDL;	//���Ű�ť��ָ��֡
static unsigned int gCanTimeStamp = 0;
static unsigned char gRecvFrameFlag = 0;
static unsigned char gHasNewFrame = 0;
static unsigned char gBtnKeyBuf[9];
static unsigned char gCopDoorType = 0;//0-ǰ�� 1-����
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void CheckCommunication(void); 
void BaseFrameRecvHandle(void);
void FrontDoorLightRecvHandle(void);
void BackDoorLightRecvHandle(void);
void DoorBtnHandle(void);
//**
// * @}
void LeagueCopCanIrqHandle(void)
{
	if(gRecvFrame.Id == LEAGUE_COP_CANID){//����ID֡
		gBaseFrame = gRecvFrame;
		gRecvFrameFlag |= ID_BASE_SHIFT;
	}else if(gRecvFrame.Id == FRONT_DOOR_LIGHTID){//ǰ�Ű�ť��ָ��
		gFrontDL = gRecvFrame;
		gRecvFrameFlag |= ID_FRONT_DL_SHIFT;
	}else if(gRecvFrame.Id == BACK_DOOR_LIGHTID){//���Ű�ť��ָ��
		gBackDL = gRecvFrame;
		gRecvFrameFlag |= ID_BACK_DL_SHIFT;
	}
	gHasNewFrame = 1;
}	
/*!
 * LeagueCopPtlInt
 * ���˽���COPЭ���ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void LeagueCopPtlInt(void)
{
	gCopCanCfg.recvId1 = LEAGUE_COP_CANID|EXT_CANFRAME_TYPE;;
	gCopCanCfg.recvId1Mask = 0x00;
	gCopCanCfg.recvId2 = FRONT_DOOR_LIGHTID|EXT_CANFRAME_TYPE;
	gCopCanCfg.recvId2Mask = 0x01;
	gCopCanCfg.recvError = 0;
	gCopCanCfg.sendError = 0;
	gCopCanCfg.ptFrame = &gRecvFrame;
	gCopCanCfg.recvIRQ = LeagueCopCanIrqHandle;
	DrvCanInit(40000,&gCopCanCfg); 
	gCanTimeStamp = DrvGetTimerMsTick();
	gCopDoorType = 0;
}
/*!
 * LeagueCopPtlHandle
 * ���˽���COPЭ�鴦��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LeagueCopPtlHandle(void)
{
	CheckCommunication();
	BaseFrameRecvHandle();
	if(gCopDoorType){//����
		BackDoorLightRecvHandle();//���ŵƴ���
	}else{
		FrontDoorLightRecvHandle();//ǰ�ŵƴ���
	}
	DoorBtnHandle();//�Ű�ť���봦��
}
/*!
 * DoorBtnHandle
 * �Ű�ť����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DoorBtnHandle(void)
{
	s_CANFRAME_T lBtnFrame;
	unsigned char lNew;
	unsigned char lCnt;
	unsigned char *ptDst;
	
	lNew = 0;
	ptDst = BtnPtlKeyStatusBuf();
	for(lCnt = 0; lCnt < 9; lCnt++){
		if(gBtnKeyBuf[lCnt] != *ptDst){
			gBtnKeyBuf[lCnt] = *ptDst;
			lNew = 1;
		}
		ptDst++;
	}
	if(lNew == 0)//��ť�ޱ仯
		return;
	lBtnFrame.Id = COP_CALL_BTNID;
	lBtnFrame.Flag = 0x08| EXT_CANFRAME_TYPE;
	lBtnFrame.Data[0] = 0xB7;
	if(gCopDoorType){//����
		lBtnFrame.Data[1] = 0x02;
	}else{
		lBtnFrame.Data[1] = 0x00;
	}
	lBtnFrame.Data[2] = gBtnKeyBuf[0];
	lBtnFrame.Data[3] = gBtnKeyBuf[1];
	lBtnFrame.Data[4] = gBtnKeyBuf[2];
	lBtnFrame.Data[5] = gBtnKeyBuf[3];
	lBtnFrame.Data[6] = 0;
	lBtnFrame.Data[7] = 0;
	if(gCopDoorType){//����
		if(BtnPtlGetKeyStatus(64)){//OBL ���Ű�ť����
			lBtnFrame.Data[6] |= 0x20;
		}else{
			lBtnFrame.Data[6] &= 0xDF;
		}
		if(BtnPtlGetKeyStatus(65)){//CBL ���Ű�ť����
			lBtnFrame.Data[6] |= 0x40;
		}else{
			lBtnFrame.Data[6] &= 0xBF;
		}		
	}else{
		if(BtnPtlGetKeyStatus(64)){//OBL ���Ű�ť����
			lBtnFrame.Data[6] |= 0x20;
		}else{
			lBtnFrame.Data[6] &= 0xDF;
		}
		if(BtnPtlGetKeyStatus(65)){//CBL ���Ű�ť����
			lBtnFrame.Data[6] |= 0x40;
		}else{
			lBtnFrame.Data[6] &= 0xBF;
		}	
		if(BtnPtlGetKeyStatus(67)){//PAS ֱʻ��ť����	
			lBtnFrame.Data[6] |= 0x02;
		}else{
			lBtnFrame.Data[6] &= 0xFD;
		}
		if(BtnPtlGetKeyStatus(68)){//ATT ˾����ť����	
			lBtnFrame.Data[6] |= 0x01;
		}else{
			lBtnFrame.Data[6] &= 0xFE;
		}	
		if(BtnPtlGetKeyStatus(71)){//FRCL ����Ա��ť��������	
			lBtnFrame.Data[7] |= 0x80;
		}else{
			lBtnFrame.Data[7] &= 0x7F;
		}
		if(BtnPtlGetKeyStatus(70)){//IND �������а�ť����
			lBtnFrame.Data[7] |= 0x10;
		}else{
			lBtnFrame.Data[7] &= 0xEF;
		}		
	}
	DrvCanSendFrame(&lBtnFrame);
	lBtnFrame.Id = COP_CALL_BTNID;
	lBtnFrame.Data[0] = 0xB7;
	if(gCopDoorType){//����
		lBtnFrame.Data[1] = 0x03;
	}else{
		lBtnFrame.Data[1] = 0x01;
	}
	lBtnFrame.Data[2] = gBtnKeyBuf[4];
	lBtnFrame.Data[3] = gBtnKeyBuf[5];
	lBtnFrame.Data[4] = gBtnKeyBuf[6];
	lBtnFrame.Data[5] = gBtnKeyBuf[7];	
	lBtnFrame.Data[6] = 0;
	DrvCanSendFrame(&lBtnFrame);
}
/*!
 * BackDoorLightRecvHandle
 * ���ŵƽ��մ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BackDoorLightRecvHandle(void)
{
	unsigned char lCnt;
	unsigned char ByteAdr,BitAdr;
	
	if((gRecvFrameFlag&ID_BACK_DL_SHIFT) == 0)//�Ǻ��ŵ�
		return;
	gRecvFrameFlag &= ID_BACK_DL_MASK;
	if((gBackDL.Flag&EXT_CANFRAME_TYPE) == 0)//����չ֡
		return;
	if(gBackDL.Id != BACK_DOOR_LIGHTID)//�Ǻ��ŵ�ID
		return;
	if((gBackDL.Flag&0x0F) != 8)//֡���Ȳ�����
		return;	
	for(lCnt = 0; lCnt < 64; lCnt++){
		ByteAdr = lCnt/8;
		BitAdr = lCnt%8;
		if(gBackDL.Data[ByteAdr]&(1<<BitAdr)){
			BtnPtlSetLightStatus(lCnt,1);
		}else{
			BtnPtlSetLightStatus(lCnt,0);
		}
	}
}
/*!
 * FrontDoorLightRecvHandle
 * ǰ�ŵƽ��մ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void FrontDoorLightRecvHandle(void)
{
	unsigned char lCnt;
	unsigned char ByteAdr,BitAdr;
	
	if((gRecvFrameFlag&ID_FRONT_DL_SHIFT) == 0)//��ǰ�ŵ�
		return;
	gRecvFrameFlag &= ID_FRONT_DL_MASK;
	if((gFrontDL.Flag&EXT_CANFRAME_TYPE) == 0)//����չ֡
		return;
	if(gFrontDL.Id != FRONT_DOOR_LIGHTID)//��ǰ�ŵ�ID
		return;
	if((gFrontDL.Flag&0x0F) != 8)//֡���Ȳ�����
		return;	
	for(lCnt = 0; lCnt < 64; lCnt++){
		ByteAdr = lCnt/8;
		BitAdr = lCnt%8;
		if(gFrontDL.Data[ByteAdr]&(1<<BitAdr)){
			BtnPtlSetLightStatus(lCnt,1);
		}else{
			BtnPtlSetLightStatus(lCnt,0);
		}
	}
}
/*!
 * BaseFrameRecvHandle
 * ����֡���մ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void BaseFrameRecvHandle(void)
{
	if((gRecvFrameFlag&ID_BASE_SHIFT) == 0)//�ǻ�����Ϣ֡
		return;
	gRecvFrameFlag &= ID_BASE_MASK;
	if((gBaseFrame.Flag&EXT_CANFRAME_TYPE) == 0)//����չ֡
		return;
	if(gBaseFrame.Id != LEAGUE_COP_CANID)//�ǻ���״̬��Ϣ֡
		return;
	if((gBaseFrame.Flag&0x0F) != 8)//֡���Ȳ�����
		return;	
	setSegLedShowFloor(gBaseFrame.Data[2],gBaseFrame.Data[3]);
	setSegLedShowArrow(gBaseFrame.Data[1]&0x07);
	setSegLedShowFire(gBaseFrame.Data[1]&0x80);
	setSegLedShowOverload(gBaseFrame.Data[4]&0x04);
	if(gCopDoorType){//����
		BtnPtlSetLightStatus(64,gBaseFrame.Data[5]&0x80);//���ſ������
		BtnPtlSetLightStatus(65,gBaseFrame.Data[5]&0x40);//���Ź������			
	}else{
		BtnPtlSetLightStatus(64,gBaseFrame.Data[5]&0x20);//ǰ�ſ������
		BtnPtlSetLightStatus(65,gBaseFrame.Data[5]&0x10);//ǰ�Ź������		
	}

}
/*!
 * CheckCommunication
 * ͨѶ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CheckCommunication(void)
{
	if(gHasNewFrame == 0){
		if((DrvGetTimerMsTick() - gCanTimeStamp) > 10000){//10S����ͨѶ
			setBoardLedWorkMode(MODE_SYSTEM_NORMAL);
		}
		return;
	}
	gHasNewFrame = 0;
	gCanTimeStamp = DrvGetTimerMsTick();
	setBoardLedWorkMode(MODE_SYSTEM_COMMUNICATION);
}
