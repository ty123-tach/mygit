/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvCan.h
	Author: ZhiweiZhang Version: V0 Date: 2019-12-10
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
#ifndef _DRVCAN_H_
#define _DRVCAN_H_
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{���к�����ݶ�������
#define EXT_CANFRAME_TYPE	0x80000000
/*�Զ���CAN֡*/
typedef struct _can_frame_{
	unsigned long Id;		/*��ʶ��*/
	unsigned long Flag;		/*����bit31�Զ���Ϊ����11λ��׼֡����29λ��չ֡*/
	unsigned char Data[8];	/*����*/
}s_CANFRAME_T; 
/*
	���磺������չ֡

			s_CANFRAME_T myFrme;

			myFrme.Id = ʵ�ʷ��͵�ID
			myFrme.Flag = ʵ�ʷ��͵����ݳ��� | EXT_CANFRAME_TYPE;
			DrvCanSendFrame(&myFrme);

		  ���ͱ�׼֡

			s_CANFRAME_T myFrme;
			
			myFrme.Id = ʵ�ʷ��͵�ID
			myFrme.Flag = ʵ�ʷ��͵����ݳ���;
			DrvCanSendFrame(&myFrme);
*/
/*�Զ����û�����*/
typedef void (*recvErrorHandle)(unsigned char error);//0-���� 1-���߾��� 2-���߹��� 3-���߹ر�
typedef void (*sendErrorHandle)(unsigned char error);
typedef void (*recvIrqHandle)(void);
typedef struct _usr_data_{
	/*recvId1����bit31�Զ���Ϊ����11λ��׼֡����29λ��չ֡*/
	unsigned long recvId1;
	unsigned long recvId1Mask;
	/*recvId2����bit31�Զ���Ϊ����11λ��׼֡����29λ��չ֡*/
	unsigned long recvId2;
	unsigned long recvId2Mask;
	s_CANFRAME_T *ptFrame;
	recvErrorHandle recvError;//���չ��ϴ���
	sendErrorHandle sendError;//���͹��ϴ���
	recvIrqHandle recvIRQ;//�����жϴ���,�������������������һ֡���ݺ����ж��д������ݣ����Զ���ô�����
	unsigned char recvStatus;
}s_USRDATA_T;
/*
	���磺���ձ�׼֡ID=0x01����չ֡ID=0x00-0x07��ѯ�÷�

		void myRecvError(unsigned char error)
		{
			do something
		}
		void mySendError(unsigned char error)
		{
			do something
		}

		s_USRDATA_T myCan;
		s_CANFRAME_T myFrme;

		myCan.recvId1 = 0x01;
		myCan.recvId1Mask = 0x00;
		myCan.recvId2 = 0x00|EXT_CANFRAME_TYPE;
		myCan.recvId2Mask = 0x07;
		myCan.recvError = myRecvError;
		myCan.sendError = mySendError;
		myCan.ptFrame = &myFrme;
		myCan.recvIRQ = 0;
		DrvCanInit(40000,&myCan);
		
		while(1){
			if(myCan.recvStatus){
				myCan.recvStatus = 0;
				//begin handle "myFrme" data struct
				if(myFrme.Flag&EXT_CANFRAME_TYPE){//extern frame
					if(myFrme.Id < 0x07){
						do something
					}
				}else{
					if(myFrme.Id == 0x01){
						do something
					}				
				}
			}
		}
*/
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{���к�����������(���ⲿ����ʹ��)
void DrvCanInit(unsigned long baudrate,s_USRDATA_T *usrdata);
void DrvCanSendFrame(const s_CANFRAME_T *ptFrame);
//**
// * @}
#endif //_DRVCAN_H_
