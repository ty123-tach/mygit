/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: CopBtnPtl.c
	Author: ZhiweiZhang Version: V0 Date: 2020-01-07
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "CopBtnPtl.h"
#include "DrvUart0.h"
#include "MKE16Z4.h"
#include "DrvTimer.h"
#include "DebugMsg.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define AUTO_SEND_SPACE 100	//�Զ����ͼ������λms
static unsigned int gCheckTimeStamp = 0;		//�ӻ����ʱ���
static unsigned int gSysSendTimeStamp = 0;		//����ʱ���

static unsigned char gSysSendBuf[22] = {0};	//�������ݻ�����
static unsigned char gSysRecvBuf[22] = {0}; //���ջ�����
static unsigned char gSysSendIndex = 0;	//��ǰ��������
static unsigned char gSysRecvIndex = 0;	//��ǰ��������
static unsigned char gSysRecvFinished = 0;

static unsigned char gUserCmdBuf[2][20] = {0};	//�û��������
static unsigned char gUserCmd = 0;
static unsigned char gSlaveKeyMap[9][8]; //�ӻ���Կӳ���
static unsigned char gSlaveValidMap[72];//�ӻ�����Чӳ��,0��1-���� 0-��� 1��1-��� 0-����
#define MAX_SLAVE_KEYPLUSE	3
static unsigned char gSlaveKeyPluse[72];//�ӻ������������
static unsigned char gSlaveKeyStatus[9];//�ӻ�����״̬

#define MAX_SALVENUM_SUPPORT 72		//���֧�ֵĴӻ���Կ����
#define MAX_IDCNT_ERROR	5			//�ӻ���Կ����������
#define MAX_SLAVE_CHECKTIME	300000	//�ӻ���Կ�������λms 5*60*1000
static unsigned char gCurQuerySlave = 0;
static unsigned char gSlaveIdError[72];//�ӻ���Կ�������
static unsigned char gSlaveInValid[9];//�ӻ���Ч״̬ 1����Ч 0����Ч
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void AutoSendDataHandle(void); 
unsigned char GetXorValue(unsigned char index);
void CmdBufClean(unsigned char index);
void SlaveKeyMapInit(void);
void SlaveKeyStatusHandle(void);
void SlaveKeyMapHandle(void);
void CheckSlaveKeyMapHandle(void);
void SlaveAdrSetHandle(void);
//**
// * @}
/*!
 * BtnPtlRWCmdInit
 * ��ťЭ���д�����ʼ��
 * @param[in]   mode��ģʽ 0-��ť�����Ա���	1-��ť���Ա���
 * @param[out]  None
 * @retval  	None
 */
void BtnPtlRWCmdInit(unsigned char mode)
{
	unsigned char lCnt;
	unsigned char lByteAdr,lBitAdr;
	
	CmdBufClean(0);
	if(mode){
		gUserCmdBuf[0][0] = 0x1F;
	}else{
		gUserCmdBuf[0][0] = 0x0F;
	}
	for(lCnt = 0; lCnt < 72; lCnt++){
		lByteAdr = lCnt/8+1;
		lBitAdr = lCnt%8;
		if(gSlaveValidMap[lCnt]){//����
			gUserCmdBuf[0][lByteAdr] |= 1<<lBitAdr;
		}
	}
	gUserCmdBuf[0][10] = GetXorValue(0);
	gUserCmd = 0;
}
/*!
 * BtnPtlReadIdCmd
 * ��ťЭ���ȡ�ӻ�ID����
 * @param[in]   adr���ӻ���ַ ��Χ0-71
 * @param[out]  None
 * @retval  	None
 */
void BtnPtlReadIdCmd(unsigned char adr)
{
	unsigned int CurrentTime;
	CmdBufClean(1);
	gUserCmdBuf[1][0] = 0x56;
	gUserCmdBuf[1][1] = adr/8+11;
	gUserCmdBuf[1][2] = adr%8;
	CurrentTime = DrvGetTimerMsTick();
	gUserCmdBuf[1][5] = CurrentTime&0xFF;
	gUserCmdBuf[1][3] = (CurrentTime>>8)&0xFF;
	gUserCmdBuf[1][4] = (CurrentTime>>16)&0xFF;
	gUserCmdBuf[1][6] = (CurrentTime>>24)&0xFF;
	gUserCmdBuf[1][10] = GetXorValue(1);
	gUserCmd = 1;	
}
/*!
 * BtnPtlAdrSetCmd
 * ��ť��ַ�趨����
 * @param[in]   adr����ǰ��Ҫ�趨�ĵ�ַ
 * @param[out]  None
 * @retval  	None
 */
void BtnPtlAdrSetCmd(unsigned char adr)
{
	unsigned char ByteAdr,BitAdr;
	unsigned char CurrentTime;
	unsigned char Temp;
	if((adr > 72)||(adr < 1))
		return;
	ByteAdr = (adr-1)/8;
	BitAdr = (adr-1)%8;
	CurrentTime = DrvGetTimerMsTick()&0xFF;
	CmdBufClean(1);	
	gUserCmdBuf[1][0] = 0x5A;
	if(gSlaveValidMap[adr-1]){//�ӷ������ַ
		gUserCmdBuf[1][1] = 0;
		Temp = ByteAdr - CurrentTime;
		gUserCmdBuf[1][2] = Temp;
		gUserCmdBuf[1][4] = CurrentTime;
		Temp = BitAdr - CurrentTime;
		gUserCmdBuf[1][3] = Temp;
		gUserCmdBuf[1][5] = CurrentTime;		
	}else{//���������ַ
		gUserCmdBuf[1][1] = 1;
		Temp = CurrentTime + ByteAdr;
		gUserCmdBuf[1][2] = Temp;
		gUserCmdBuf[1][4] = CurrentTime;	
		Temp = CurrentTime + BitAdr;		
		gUserCmdBuf[1][3] = Temp;
		gUserCmdBuf[1][5] = CurrentTime;
	}
	gUserCmdBuf[1][10] = GetXorValue(1);
	gUserCmd = 1;
}
/*!
 * CopBtnPtlInit
 * COP��ťЭ���ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CopBtnPtlInit(void)
{
	DrvUart0Init();
	SlaveKeyMapInit();
	gSysSendTimeStamp = DrvGetTimerMsTick();
	gSysSendIndex = 0;
	gSysRecvIndex = 0;
	gSysRecvFinished = 0;
	BtnPtlRWCmdInit(1);
	gUserCmd = 0;
	gCurQuerySlave = 0;
	gCheckTimeStamp = DrvGetTimerMsTick();
}
/*!
 * CopBtnPtlHandle
 * COP��ťЭ�鴦��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CopBtnPtlHandle(void)
{
	SlaveAdrSetHandle();
	CheckSlaveKeyMapHandle();
	SlaveKeyMapHandle();
	SlaveKeyStatusHandle();
	AutoSendDataHandle();
}
/*!
 * SlaveAdrSetHandle
 * �ӻ���ַ�趨��Ӧ����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SlaveAdrSetHandle(void)
{
	unsigned char lCnt,lIndex;
	unsigned char lData;
	unsigned char lShift;	
	if(gSysRecvFinished == 0)//δ������һ֡����
		return;
	if(gSysRecvBuf[0] != 0x5A)//�ǵ�ַ�趨֡
		return;
	gSysRecvFinished = 0;
	for(lCnt = 0; lCnt < 9; lCnt++){
		lData = gSysRecvBuf[11+lCnt];
		for(lIndex = 0; lIndex < 8; lIndex++){
			lShift = 1<<lIndex;
			if(lData&lShift){//�����а���
				gSlaveKeyStatus[lCnt] |= lShift;
			}else{
				gSlaveKeyStatus[lCnt] &= ~lShift;
			}
		}
	}	
}
/*!
 * BtnPtlGetKeyStatus
 * ��ȡ�ӻ�����״̬
 * @param[in]  	adr���ӻ���ַ
 * @param[out]  None
 * @retval  	None
 */
unsigned char BtnPtlGetKeyStatus(unsigned char adr)
{
	unsigned char ByteAdr,BitAdr;
	
	if(adr > 71)
		return 0;
	ByteAdr = adr/8;
	BitAdr = adr%8;
	if(gSlaveInValid[ByteAdr]&(1<<BitAdr))//��ť��Ч
		return 0;
	return gSlaveKeyStatus[ByteAdr]&(1<<BitAdr);
}
/*!
 * BtnPtlKeyStatusBuf
 * ��ȡ�ӻ�����״̬������
 * @param[in]  	None
 * @param[out]  None
 * @retval  	��������ַ
 */
unsigned char *BtnPtlKeyStatusBuf(void)
{
	return gSlaveKeyStatus;
}
/*!
 * BtnPtlSetLightStatus
 * ���ð�ť��״̬
 * @param[in]  	adr���ӻ���ַ	sta���ӻ���״̬
 * @param[out]  None
 * @retval  	None
 */
void BtnPtlSetLightStatus(unsigned char adr,unsigned char sta)
{
	unsigned char ByteAdr,BitAdr;
	if(adr > 71)
		return;
	ByteAdr = adr/8;
	BitAdr = adr%8;
	if(gSlaveInValid[ByteAdr]&(1<<BitAdr))//��ť��Ч
		return;	
	ByteAdr += 1;
	gUserCmdBuf[0][0] = 0x1F;
	if(gSlaveValidMap[adr]){//����,0��� 1���
		if(sta){//����
			gUserCmdBuf[0][ByteAdr] &= ~(1<<BitAdr);
		}else{
			gUserCmdBuf[0][ByteAdr] |= (1<<BitAdr);
		}	
	}else{//ż��,1���,0���
		if(sta){//����
			gUserCmdBuf[0][ByteAdr] |= (1<<BitAdr);
		}else{
			gUserCmdBuf[0][ByteAdr] &= ~(1<<BitAdr);
		}		
	}
	gUserCmdBuf[0][10] = GetXorValue(0);
	gUserCmd = 0;	
}
/*!
 * CheckSlaveKeyMapHandle
 * �ӻ���Կ����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CheckSlaveKeyMapHandle(void)
{
	if((DrvGetTimerMsTick()-gCheckTimeStamp) < MAX_SLAVE_CHECKTIME)
		return;
	BtnPtlReadIdCmd(gCurQuerySlave);
}
/*!
 * SlaveKeyMapHandle
 * �ӻ���Կӳ�䴦��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SlaveKeyMapHandle(void)
{
	unsigned char ByteAdr;
	unsigned char BitAdr;
	if(gSysRecvFinished == 0)//δ������һ֡����
		return;
	if(gSysRecvBuf[0] != 0x56)//����Կ����֡
		return;
	gSysRecvFinished = 0;
	ByteAdr = gCurQuerySlave/8;
	BitAdr = gCurQuerySlave%8;	
	if(gSysRecvBuf[15] != gSlaveKeyMap[ByteAdr][BitAdr]){//��Կ����ȷ
		gSlaveIdError[gCurQuerySlave]++;
		if(gSlaveIdError[gCurQuerySlave] > MAX_IDCNT_ERROR){//�����������
			gSlaveIdError[gCurQuerySlave] = MAX_IDCNT_ERROR;
			gSlaveInValid[ByteAdr] |= 1<<BitAdr;//��Ǵӻ���Ч
		}
	}else{
		gSlaveIdError[gCurQuerySlave] = 0;
		gSlaveInValid[ByteAdr] &= ~(1<<BitAdr);
	}
	gCurQuerySlave++;
	if(gCurQuerySlave >= MAX_SALVENUM_SUPPORT){
		gCurQuerySlave =0;
	}	
	gCheckTimeStamp = DrvGetTimerMsTick();
	gUserCmd = 0;//�л��س��水ť��ȡָ��	
}
/*!
 * SlaveKeyStatusHandle
 * �ӻ���ť״̬����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SlaveKeyStatusHandle(void)
{
	unsigned char lCnt,lIndex;
	unsigned char lData;
	unsigned char lPluse;
	unsigned char lShift;
	if(gSysRecvFinished == 0)//δ������һ֡����
		return;
	if(gSysRecvBuf[0] != 0x1F)//��������ť֡
		return;
	gSysRecvFinished = 0;
	for(lCnt = 0; lCnt < 9; lCnt++){
		lData = gSysRecvBuf[11+lCnt];
		lPluse = lCnt*8;
		for(lIndex = 0; lIndex < 8; lIndex++){
			lShift = 1<<lIndex;
			if(lData&lShift){//�����а���
				if((gSlaveInValid[lCnt]&lShift) == 0){//��ť��Ч
					gSlaveKeyPluse[lPluse]++;
					if(gSlaveKeyPluse[lPluse] > MAX_SLAVE_KEYPLUSE){
						gSlaveKeyPluse[lPluse] = MAX_SLAVE_KEYPLUSE;
						gSlaveKeyStatus[lCnt] |= lShift;
					}
				}
			}else{
				gSlaveKeyStatus[lCnt] &= ~lShift;
				gSlaveKeyPluse[lPluse] = 0;
			}
			lPluse++;
		}
	}
}
/*!
 * AutoSendDataHandle
 * �����Զ��������ݴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void AutoSendDataHandle(void)
{	
	unsigned char lCnt;
	if(((DrvGetTimerMsTick()-gSysSendTimeStamp) < AUTO_SEND_SPACE)||(gSysRecvFinished == 1)){
		if(gSysRecvFinished){
			if((DrvGetTimerMsTick()-gSysSendTimeStamp) > 3000)//3Sδ�����л�
				gSysRecvFinished = 0;
		}
		return;
	}
	gSysSendTimeStamp = DrvGetTimerMsTick();
	gSysSendIndex = 0;
	gSysRecvIndex = 0;
	for(lCnt = 0; lCnt < 20; lCnt++){
		gSysSendBuf[lCnt] = gUserCmdBuf[gUserCmd][lCnt];
//		DebugMsg("%02X ",gSysRecvBuf[lCnt]);
	}
//	DebugMsg("\r\n");
	LPUART0->FIFO |= LPUART_FIFO_RXFLUSH_SHIFT|LPUART_FIFO_TXFLUSH_SHIFT;
	LPUART0->CTRL |= LPUART_CTRL_TIE_MASK;	
}
/*!
 * LPUART0_IRQHandler
 * ����0�ж�
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LPUART0_IRQHandler(void)
{
	uint32_t status;
	status = LPUART0->STAT;
	if(status&LPUART_STAT_RDRF_MASK){//�����ж�
		gSysRecvBuf[gSysRecvIndex] = LPUART0->DATA;
		gSysRecvIndex++;
		if(gSysRecvIndex >= 20){
			gSysRecvIndex = 20;
			gSysRecvFinished = 1;
		}
	}else if(status&LPUART_STAT_TDRE_MASK){//�����ж�
		if(gSysSendIndex < 20){
			LPUART0->DATA = gSysSendBuf[gSysSendIndex];
			gSysSendIndex++;	
			gSysSendTimeStamp = DrvGetTimerMsTick();
		}else{
			LPUART0->CTRL = (LPUART0->CTRL & (~LPUART_CTRL_TIE_MASK));
		}	
	}	
}
/*!
 * GetXorValue
 * ��ȡ���У����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
unsigned char GetXorValue(unsigned char index)
{
	unsigned char lCnt;
	unsigned char lRes = 0;

	for(lCnt = 0; lCnt < 10;lCnt++){
		lRes = lRes ^ gUserCmdBuf[index][lCnt];
	}
	return lRes;
}
/*!
 * CmdBufClean
 * ����������
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void CmdBufClean(unsigned char index)
{
	unsigned char lCnt;
	for(lCnt = 0; lCnt < 20; lCnt++){
		gUserCmdBuf[index][lCnt] = 0;
	}
}
/*!
 * SlaveKeyMapInit
 * �ӻ���ֵӳ���ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SlaveKeyMapInit(void)
{
	unsigned char lTable[8] = {0xa7,0xd9,0x29,0x5a,0x8b,0x61,0xc3,0xf4};
	unsigned char lCnt,i,lSum;
	unsigned char *ptKey;

	for(lCnt = 0; lCnt < 9;lCnt++){
		for(i = 0; i < 8;i++){
			gSlaveKeyMap[lCnt][i] = lTable[i]-i+lCnt;
		}
	}
	ptKey = gSlaveKeyMap[0];
	for(lCnt = 0; lCnt < 72;lCnt++){
		lSum = 0;
		for(i = 0; i < 8; i++){
			if(*ptKey&(1<<i)){
				lSum++;
			}
		}
		if(lSum&0x01){//����
			gSlaveValidMap[lCnt] = 1;
		}else{//ż��
			gSlaveValidMap[lCnt] = 0;
		}
		ptKey++;
	}
} 
