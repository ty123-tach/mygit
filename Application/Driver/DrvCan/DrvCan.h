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
//* @{引用头文件声明区域
 
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{公有宏和数据定义区域
#define EXT_CANFRAME_TYPE	0x80000000
/*自定义CAN帧*/
typedef struct _can_frame_{
	unsigned long Id;		/*标识符*/
	unsigned long Flag;		/*利用bit31自定义为区别11位标准帧还是29位扩展帧*/
	unsigned char Data[8];	/*数据*/
}s_CANFRAME_T; 
/*
	例如：发送扩展帧

			s_CANFRAME_T myFrme;

			myFrme.Id = 实际发送的ID
			myFrme.Flag = 实际发送的数据长度 | EXT_CANFRAME_TYPE;
			DrvCanSendFrame(&myFrme);

		  发送标准帧

			s_CANFRAME_T myFrme;
			
			myFrme.Id = 实际发送的ID
			myFrme.Flag = 实际发送的数据长度;
			DrvCanSendFrame(&myFrme);
*/
/*自定义用户数据*/
typedef void (*recvErrorHandle)(unsigned char error);//0-正常 1-总线警告 2-总线故障 3-总线关闭
typedef void (*sendErrorHandle)(unsigned char error);
typedef void (*recvIrqHandle)(void);
typedef struct _usr_data_{
	/*recvId1利用bit31自定义为区别11位标准帧还是29位扩展帧*/
	unsigned long recvId1;
	unsigned long recvId1Mask;
	/*recvId2利用bit31自定义为区别11位标准帧还是29位扩展帧*/
	unsigned long recvId2;
	unsigned long recvId2Mask;
	s_CANFRAME_T *ptFrame;
	recvErrorHandle recvError;//接收故障处理
	sendErrorHandle sendError;//发送故障处理
	recvIrqHandle recvIRQ;//接收中断处理,如需在驱动层接收完整一帧数据后在中断中处理数据，可自定义该处理函数
	unsigned char recvStatus;
}s_USRDATA_T;
/*
	例如：接收标准帧ID=0x01和扩展帧ID=0x00-0x07轮询用法

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
//* @{公有函数声明区域(供外部调用使用)
void DrvCanInit(unsigned long baudrate,s_USRDATA_T *usrdata);
void DrvCanSendFrame(const s_CANFRAME_T *ptFrame);
//**
// * @}
#endif //_DRVCAN_H_
