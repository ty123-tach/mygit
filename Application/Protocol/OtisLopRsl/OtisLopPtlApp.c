/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: OtisLopPtlApp.c
	Author: ZhiweiZhang Version: V0 Date: 2019-10-24
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "OtisLopPtlApp.h"
#include "OtisHbRsl.h"
#include "RslHbGpio.h"
#include "OtisHpiRsl.h"
#include "DrvTimer.h"
#if NO_SUPORT_LCD_SHOW
#include "OtisLcd.h"
#endif
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define SYS_CFG_HPISPEC_ADR	0x00002400	//HPI�ӿ�������ʾ(��ͷ&����)֡��ַ�洢��Flash�е�λ��
#define SYS_CFG_HBLOCK_ADR 	0x00002800	//HB�ӿ�����֡��ַ�洢��Flash�е�λ��


static unsigned char gCurHbPortAdress;	//��ǰHB�ӿڵ�ַ
static unsigned char gLastLockStatus;	//��һ������״̬
static unsigned char gLastRecevData;	//��һ��HB�ӿڵĽ�������
static unsigned char gLastSendData;		//��һ��HB�ӿڵķ�������


#define SEG_FILTER_SIZE	1
static unsigned char gSpecSegBuf[SEG_FILTER_SIZE];	//������ʾ�λ�����
static unsigned char gSpecStableSeg;	//�˲����������ʾ���������ݶ�
static unsigned char gSegBufFilter;	//�˲�����
static unsigned char gCurHpiAddress;	//��ǰ��HPI��ͷ��ʾ��ַ
static unsigned char gHpiFrameBuf[64];	//HPI֡���ݻ�����
static unsigned long gFloorSegBuf[SEG_FILTER_SIZE];	//¥����ʾ�λ�����
static unsigned long gFloorStableSeg;	//�˲����������ʾ��¥�������
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void HbPortAdressHandle(void);
void HbPortButtonHandle(void);
void HbLockButtonHandle(void);
extern void ZkMemCpy(void *dst,void *src,const unsigned long len);
void OtisRslSegFilterHandle(void);
unsigned long GetFloorSeg(void);
unsigned char GetSpecSeg(void);
void OtisHpiReadData(void);
#if NO_SUPORT_LCD_SHOW
void OtisRslSpecDataHandle(void);
void OtisRslFloorSegHandle(void);
#endif
void CleanHpiFrame(void);
//**
// * @}
/*!
 * OtisRslGetFloorSeg
 * ��ȡ¥����ʾ��λ
 * @param[in]   None
 * @param[out]  None
 * @retval  	¥���λ
 */
unsigned long OtisRslGetFloorSeg(void)
{
	return gFloorStableSeg;
}
/*!
 * OtisRslGetLiftStatus
 * ��ȡ��������״̬
 * @param[in]   None
 * @param[out]  None
 * @retval  	0��������	��0��������
 */
unsigned char OtisRslGetLiftStatus(void)
{
	return gSpecStableSeg;
} 
/*!
 * OtisLopPtlAppInit
 * LOPЭ��Ӧ�ó�ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void OtisLopPtlAppInit(void)
{
	unsigned char lLockAdr;
	//HB�ӿڳ�ʼ��
	OtisHbRslInit();
	RslHbGpioInit();
	gCurHbPortAdress = GetHbGpioDevAdr();
	if(gCurHbPortAdress < 4){
		SetHbPortSendData(0);
		SetHbDnButtonStatus(RSL_LIGHT_OFF);
		SetHbUpButtonStatus(RSL_LIGHT_OFF);
	}else{	
		SetHbPortAddress(gCurHbPortAdress);
	}
	//HB���ݳ�ʼ��
	lLockAdr = *((unsigned char *)SYS_CFG_HBLOCK_ADR);//��Flash�ϼ�������֡��ַ
	if((lLockAdr < 68)||(lLockAdr > 127))//�Ƿ���ַ
		lLockAdr = 124;//�趨ΪĬ�ϵ�ַ
	SetHbLockAddress(lLockAdr);
	gLastLockStatus = GetHbGpioKeyStatus(RSL_LOCKBTN_KEY);
	if(gLastLockStatus){
		SetHbLockSendData(0x11);
	}else{
		SetHbLockSendData(0);
	}
	gLastRecevData = 0;
	gLastSendData = 0;
	//HPI�ӿڳ�ʼ��
	OtisHpiRslInit();
	gCurHpiAddress = *((unsigned char *)SYS_CFG_HPISPEC_ADR);//��Flash�ϼ���HPI������ʾ֡��ַ
	if(gCurHpiAddress > 63)//�Ƿ���ַ
		gCurHpiAddress = 56;//�趨ΪĬ�ϵ�ַ
	gFloorStableSeg = 0;
	gSpecStableSeg = 0;	
}
/*!
 * OtisLopPtlAppHandle
 * LOPЭ��Ӧ�ô���
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void OtisLopPtlAppHandle(void)
{	
	RslHbGpioInputHandle();
	HbPortAdressHandle();
	HbPortButtonHandle();
	HbLockButtonHandle();
	OtisHpiReadData();
	if(GetHpiTimeOut()){//��ʾͨѶ��ʱ
		gFloorStableSeg = 0;
		gSpecStableSeg = 0;
		HpiReceiveBufferClean();
		CleanHpiFrame();
	}
#if NO_SUPORT_LCD_SHOW 	
	OtisRslSpecDataHandle();
	OtisRslFloorSegHandle();
#endif	
}
/*!
 * CleanHpiFrame
 * LOPЭ��Ӧ�ô���
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void CleanHpiFrame(void)
{
	unsigned char lCnt;
	
	for(lCnt = 0; lCnt < 64; lCnt++){
		gHpiFrameBuf[lCnt] = 0;
	}
}
/*!
 * OtisRslSpecDataHandle
 * OTIS����֡(��ͷ/����)���ݴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
#if NO_SUPORT_LCD_SHOW 
void OtisRslSpecDataHandle(void)
{	
	OtisRslLcdStopSegShow(gSpecStableSeg&0x01);
	OtisRslLcdFullSegShow(gSpecStableSeg&0x02);
	if(gSpecStableSeg&0x04){
		OtisRslLcdArrowUpSegShow(1);//����
	}else if(gSpecStableSeg&0x08){
		OtisRslLcdArrowDnSegShow(1);//����		
	}else{
		OtisRslLcdArrowUpSegShow(0);//����
		OtisRslLcdArrowDnSegShow(0);//����
	}
}
#endif
/*!
 * OtisRslFloorSegHandle
 * OTIS¥������ݴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
#if NO_SUPORT_LCD_SHOW 
void OtisRslFloorSegHandle(void)
{
	unsigned char lCnt;
	unsigned char lSegH = 0;
	unsigned char lSegL = 0;
	for(lCnt = 0; lCnt < 16; lCnt++){
		if(gFloorStableSeg&(0x10000<<lCnt)){
			lSegH = 1;
		}else{
			lSegH = 0;
		}
		if(gFloorStableSeg&(1<<lCnt)){
			lSegL = 1;
		}else{
			lSegL = 0;
		}
		OtisRslLcdFloorShowSeg(lCnt,lSegL,lSegH);	
	}
}
#endif
/*!
 * OtisHpiReadData
 * ��ȡHPI����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void OtisHpiReadData(void)
{
	unsigned char *ptDat;	
	
	ptDat = GetHpiReceiveData();
	if(ptDat == 0)
		return;
	ZkMemCpy(gHpiFrameBuf,ptDat,64);
	OtisRslSegFilterHandle();
}
/*!
 * OtisRslSegFilterHandle
 * �����ݼ����˲�����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void OtisRslSegFilterHandle(void)
{
	unsigned char lCnt;
	unsigned char lIsSpecSame = 1;
	unsigned char lIsFloorSame = 1;
	unsigned char lSize;
	
	gFloorSegBuf[gSegBufFilter] = GetFloorSeg();
	gSpecSegBuf[gSegBufFilter] = GetSpecSeg();
	gSegBufFilter++;
	if(gSegBufFilter < SEG_FILTER_SIZE)
		return;
	gSegBufFilter = 0;
	lSize = SEG_FILTER_SIZE - 1;
	for(lCnt = 0; lCnt < lSize; lCnt++){
		if(gFloorSegBuf[lCnt] != gFloorSegBuf[lCnt+1]){
			lIsFloorSame = 0;
		}
		if(gSpecSegBuf[lCnt] != gSpecSegBuf[lCnt+1]){
			lIsSpecSame = 0;
		}		
	}
	if(lIsFloorSame)
		gFloorStableSeg = gFloorSegBuf[lSize];
	if(lIsSpecSame)
		gSpecStableSeg = gSpecSegBuf[lSize];
}
/*!
 * GetFloorSeg
 * ��ȡ¥�������
 * @param[in]   None
 * @param[out]  None
 * @retval  	��16λ-��λ¥���	��16λ-��λ¥���
 */
unsigned long GetFloorSeg(void)
{
	unsigned long lSeg = 0;
	unsigned char lCnt;
	unsigned char lStartPos = 6;
	
	for(lCnt = 0; lCnt < 16; lCnt++){
		if(lCnt > 7){
			lStartPos = 22;
		}
		if(gHpiFrameBuf[lStartPos+lCnt]&0x10){
			lSeg |= 1<<lCnt;
		}
		if(gHpiFrameBuf[14+lCnt]&0x10){
			lSeg |= 0x10000<<lCnt;
		}	
	}
	return lSeg;
}
/*!
 * GetSpecSeg
 * ��ȡ�������ݶ�
 * @param[in]   None
 * @param[out]  None
 * @retval  	�������ݶ�����
 */
unsigned char GetSpecSeg(void)
{
	unsigned char lSpec = 0;

	if(gCurHpiAddress == 60){
		lSpec = (gHpiFrameBuf[gCurHpiAddress]&0x03)<<2;
		if(gHpiFrameBuf[gCurHpiAddress]&0x04)
			lSpec |= 0x02;
		if(gHpiFrameBuf[gCurHpiAddress]&0x08)
			lSpec |= 0x01;
		return lSpec;
	}
	return gHpiFrameBuf[gCurHpiAddress];
}
/*!
 * HbLockButtonHandle
 * ���ݰ�ť����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HbLockButtonHandle(void)
{
	unsigned char lSta;
	
	lSta = GetHbGpioKeyStatus(RSL_LOCKBTN_KEY);
	if(gLastLockStatus == lSta)
		return;
	gLastLockStatus = lSta;
	if(gLastLockStatus){
		SetHbLockSendData(0x11);
	}else{
		SetHbLockSendData(0);
	}	
}
/*!
 * HbPortButtonHandle
 * LOP��ť����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HbPortButtonHandle(void)
{
	unsigned char lHbRecv;
	unsigned char lHbSend;
	
	if(gCurHbPortAdress < 4){//��������Ч��ַ
		return;
	}
	lHbRecv = GetHbPortReceiveData();
	if(lHbRecv != gLastRecevData){
		gLastRecevData = lHbRecv;
		if(lHbRecv&0x1){//���е���
			SetHbUpButtonStatus(RSL_LIGHT_ON);
		}else{
			SetHbUpButtonStatus(RSL_LIGHT_OFF);
		}
		if(lHbRecv&0x2){//���е���
			SetHbDnButtonStatus(RSL_LIGHT_ON);
		}else{
			SetHbDnButtonStatus(RSL_LIGHT_OFF);
		}		
	}
	//���Ͱ�ť��Ϣ����
	lHbSend = GetHbButtonValue();
	if(lHbSend != gLastSendData){
		gLastSendData = lHbSend;
		lHbSend = 0;
		if(gLastSendData&RSL_UPBTN_KEY){
			lHbSend |= 0x11;
		}
		if(gLastSendData&RSL_DNBTN_KEY){//�°�ť����
			if(lHbSend == 0){//�ϰ�ťδ����
				lHbSend |= 0x12;//Bit5У��λΪ1
			}else{
				lHbSend = 0x03;//Bit5У��λΪ0
			}
		}
		SetHbPortSendData(lHbSend);		
	}
}
/*!
 * HbPortAdressHandle
 * HB�ӿڵ�ַ����
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void HbPortAdressHandle(void)
{
	unsigned char lHbPortAdr;
	
	lHbPortAdr = GetHbGpioDevAdr();
	if(lHbPortAdr != gCurHbPortAdress){
		gCurHbPortAdress = lHbPortAdr;
		if(gCurHbPortAdress < 4){
			SetHbPortSendData(0);
			SetHbDnButtonStatus(RSL_LIGHT_OFF);
			SetHbUpButtonStatus(RSL_LIGHT_OFF);
		}else{
			SetHbPortAddress(gCurHbPortAdress);
		}
	}	
}
