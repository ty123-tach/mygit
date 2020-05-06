/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: OtisHbRsl.c
	Author: ZhiweiZhang Version: V0 Date: 2019-10-24
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "OtisHbRsl.h"
#include "System.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
/*ƽ̨���HB��ʱ��ֵ*/
#define HB_TIMER_100US_VALUE	14850	//100us��ʱ��ֵ
#define HB_TIMER_200US_VALUE	14700	//200us��ʱ��ֵ
#define HB_TIMER_270US_VALUE	14595	//270us��ʱ��ֵ
#define HB_TIMER_1P2MS_VALUE	1800	//1.2ms��ʱ��ֵ
#define HB_TIMER_10MS_VALUE		0		//10ms��ʱ��ֵ
/*	ƽ̨��ص�HB��ʱ��ֹͣ*/
#define LOP_HB_TIMERSTOP 	(FTM1->SC &= 0xFFFFFFE7)
/*	ƽ̨��ص�HB��ʱ����λ100us*/
#define LOP_HB_RESET100US	FTM1->SC &= 0xFFFFFFE7; \
							FTM1->CNTIN = HB_TIMER_100US_VALUE;	\
							FTM1->CNT = 0;			\
							FTM1->SC |= 0x08
/*	ƽ̨��ص�HB��ʱ����λ200us*/
#define LOP_HB_RESET200US	FTM1->SC &= 0xFFFFFFE7; \
							FTM1->CNTIN = HB_TIMER_200US_VALUE;	\
							FTM1->CNT = 0;			\
							FTM1->SC |= 0x08
/*	ƽ̨��ص�HB��ʱ����λ270us*/
#define LOP_HB_RESET270US	FTM1->SC &= 0xFFFFFFE7; \
							FTM1->CNTIN = HB_TIMER_270US_VALUE;	\
							FTM1->CNT = 0;			\
							FTM1->SC |= 0x08
/*	ƽ̨��ص�HB��ʱ����λ10ms*/
#define LOP_HB_RESET10MS	FTM1->SC &= 0xFFFFFFE7; \
							FTM1->CNTIN = HB_TIMER_10MS_VALUE;	\
							FTM1->CNT = 0;			\
							FTM1->SC |= 0x08							
/*	ƽ̨��ص�HPI��ʱ����ȡTCֵ*/
#define LOP_HB_READTC		(FTM1->CNT)

/*	ƽ̨��ص�GPIO���DA_B_OUT���ϸߵ�ƽ*/
#define LOP_HBDA_OUTH 	GPIOE->PCOR = 0x04	//оƬGPIO������,������������
/*	ƽ̨��ص�GPIO���DA_B_OUT���ϵ͵�ƽ*/
#define LOP_HBDA_OUTL	GPIOE->PSOR = 0x04	//оƬGPIO������,������������
/*	ƽ̨��ص�GPIO��ȡDA_A���ŵ������ƽ״̬*/
#define PLATFORM_HBDA_READ	(GPIOE->PDIR&GPIOPORT_PIN1_MASK)	//��ȡDA������GPIO������	
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void HbPortTimerInit(void);
void HbPortGpioInit(void);
void HbPortRamInit(void);
__STATIC_INLINE void HbPortRisingHandle(void);
__STATIC_INLINE void HbPortTimerHandle(void);
//**
// * @}
volatile unsigned char gHbDatSampleFlag;//HB�ӿ����ݲ�����־
volatile unsigned char gHbSampleCnt;//HB�ӿ����ݲ�������
volatile unsigned char gIsHbClkSync;//ʱ��ͬ��״̬
volatile unsigned char gHbClkCnt;//HB�ӿ�ʱ�Ӹ���
volatile unsigned char gHbTimeOut = 0;//HB�ӿڳ�ʱ
volatile unsigned char gHbPortReadAdress;//HB�ӿڶ�ȡ��ַ
volatile unsigned char gHbPortRecvData[2];//HB�ӿڽ��յ�����
volatile unsigned char gHbPortWriteAdress;//HB�ӿ�д���ݵ�ַ
volatile unsigned char gHbPortSendData[2];//HB�ӿڷ��͵�����
volatile unsigned char gHbLockAdress = 0;//HB�ӿ����ݵ�ַ
volatile unsigned char gHbLockSendData[2];//HB�ӿ����ݷ�������
/*!
 * OtisHbRslInit
 * HB�ӿڳ�ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void OtisHbRslInit(void)
{
	HbPortTimerInit();
	HbPortGpioInit();
	HbPortRamInit();
}
/*!
 * GetHbPortTimeOutSta
 * ��ȡHB�ӿڳ�ʱ״̬
 * @param[in]   None
 * @param[out]  None
 * @retval  	1����ʱ	0���޳�ʱ
 */
unsigned char GetHbPortTimeOutSta(void)
{
	return gHbTimeOut;
}
/*!
 * GetHbPortReceiveData
 * ��ȡHB�ӿڽ��յ�����
 * @param[in]   None
 * @param[out]  None
 * @retval  	���յ�������
 */
unsigned char GetHbPortReceiveData(void)
{
	return gHbPortRecvData[1];
}
/*!
 * SetHbPortSendData
 * ����HB�ӿڷ�������
 * @param[in]   data�������͵�����
 * @param[out]  None
 * @retval  	None
 */
void SetHbPortSendData(unsigned char data)
{
	gHbPortSendData[1] = data;
}
/*!
 * SetHbPortAddress
 * ����HB�ӿڵ�ַ
 * @param[in]   adr���ӿڵ�ֵַ
 * @param[out]  None
 * @retval  	None
 */
void SetHbPortAddress(unsigned char adr)
{
	if(gHbPortReadAdress == adr)
		return;
	if(adr >= 64)
		return;
	gHbPortReadAdress = adr;
	gHbPortWriteAdress = adr+64;
}
/*!
 * SetHbLockSendData
 * ����HB��������
 * @param[in]   data�������͵�����
 * @param[out]  None
 * @retval  	None
 */
void SetHbLockSendData(unsigned char data)
{
	gHbLockSendData[1] = data;
}
/*!
 * SetHbLockAddress
 * ����HB���ݵ�ַ
 * @param[in]   adr����ֵַ
 * @param[out]  None
 * @retval  	None
 */
void SetHbLockAddress(unsigned char adr)
{
	if(gHbLockAdress == adr)
		return;
	if((adr >= 128)||(adr < 68))
		return;
	gHbLockAdress = adr;
}
/*!
 * HbPortRamInit
 * HB�ӿ�RAM���ݳ�ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void HbPortRamInit(void)
{
	gHbDatSampleFlag = 0;
	gHbSampleCnt = 0;
	gIsHbClkSync = 0;
	gHbClkCnt = 0;
	gHbTimeOut = 0;
	gHbPortReadAdress = 0;
	gHbPortRecvData[0] = 0;
	gHbPortRecvData[1] = 0;
	gHbPortWriteAdress = 64;
	gHbPortSendData[0] = 0;
	gHbPortSendData[1] = 0;	
	gHbLockAdress = 124;
	gHbLockSendData[0] = 0;
	gHbLockSendData[1] = 0;	
}
/*!
 * FTM1_IRQHandler
 * LOP HB ��ʱ���ж�
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void FTM1_IRQHandler(void)
{
	if(FTM1->SC&0x200){
		FTM1->SC &= 0xFDFF;
		HbPortTimerHandle();
	}
}
/*!
 * PORTBCD_IRQHandler
 * LOP HB CLK�������ж�
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void PORTAE_IRQHandler(void)
{
	PORTE->ISFR = GPIOPORT_PIN0_MASK;
	HbPortRisingHandle();
}
/*!
 * HbPortTimerHandle
 * LOP HB�ӿڶ�ʱ���жϴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
__STATIC_INLINE void HbPortTimerHandle(void)
{
	if(gIsHbClkSync == 0){//ʱ�ӵȴ�ͬ����ʱ
		LOP_HB_TIMERSTOP;
		gHbTimeOut = 1;
		return;
	}
	if(gHbDatSampleFlag == 0){//����������ʱ
		LOP_HB_TIMERSTOP;
		gHbTimeOut = 1;
		return;
	}
	gHbTimeOut = 0;	
	if(gHbClkCnt < 64){/*���ƹ���ʱ�䴰��*/
		if(gHbSampleCnt == 0){//CLK������ʱ�ӵ����������ʱ����ĵ�һ��������
			LOP_HB_RESET100US;//���Ķ�ʱ��100us����һ��
		}		
		if(PLATFORM_HBDA_READ){//DA������Ϊ��
			gHbPortRecvData[0] |= 1<<gHbSampleCnt;
		}
		gHbSampleCnt++;//���ݲ��������
		if(gHbSampleCnt < 5)//��������δ����
			return;
		if(gHbPortReadAdress == gHbClkCnt){//����ַƥ��
			if((gHbPortRecvData[0]&0x0F) != gHbPortRecvData[1])
				gHbPortRecvData[1] = gHbPortRecvData[0]&0x0F;//�����������
		}
		gHbClkCnt++;		
	}else{				/*���ƹ��ȡ�ӻ�����ʱ�䴰��*/
		if((gHbPortWriteAdress == gHbClkCnt)||(gHbLockAdress == gHbClkCnt)){//д��ַƥ��
			if(gHbSampleCnt < 5){//��������δ����
				if(gHbSampleCnt == 0){//CLK������ʱ�ӵ����������ʱ����ĵ�һ��������
					if(gHbPortWriteAdress == gHbClkCnt){
						if(gHbPortSendData[0] != gHbPortSendData[1]){
							gHbPortSendData[0] = gHbPortSendData[1];
						}				
					}else{
						if(gHbLockSendData[0] != gHbLockSendData[1]){
							gHbLockSendData[0] = gHbLockSendData[1];
						}						
					}
					LOP_HB_RESET100US;//���Ķ�ʱ��100us����һ��
				}
				if(gHbPortWriteAdress == gHbClkCnt){
					if(gHbPortSendData[0]&(1<<gHbSampleCnt)){
						LOP_HBDA_OUTH;
					}else{
						LOP_HBDA_OUTL;
					}					
				}else{
					if(gHbLockSendData[0]&(1<<gHbSampleCnt)){
						LOP_HBDA_OUTH;
					}else{
						LOP_HBDA_OUTL;
					}					
				}
				gHbSampleCnt++;//���ݲ��������
				return;
			}		
		}
		LOP_HBDA_OUTL;
		gHbClkCnt++;
		if(gHbClkCnt >= 128){//ʱ�Ӹ����ﵽЭ�鶨���128֡
			gIsHbClkSync = 0;//��Ϊ�ȴ���һ����ͬ��
		}		
	}
	gHbDatSampleFlag = 0;
	LOP_HB_RESET10MS;
}
/*!
 * HbPortRisingHandle
 * LOP HB�ӿ��������жϴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
__STATIC_INLINE void HbPortRisingHandle(void)
{
	if(gHbDatSampleFlag)//���ݲ�����,��ʵ������з���,RSL���ߵ�CLK����MCU��
		return;			//����ʱ���½���ʱ����ּ�嶶��������MCU���ٽ���һ���ж�
	if(gIsHbClkSync == 0){//ʱ�ӵȴ�ͬ��
		//��ǰTC��ֵС��1.2ms
		if(LOP_HB_READTC < HB_TIMER_1P2MS_VALUE){//Э��ͬ��ʱ��Ϊ1.6ms,֡ʱ����Ϊ0.8ms,ȡ1.2ms��Ϊ���ĵ�
			LOP_HB_RESET10MS;//��λ��ʱ��			
			return;//ͬ��ʱ��δ��,����				
		}
		gIsHbClkSync = 1;//ʱ����ͬ��
		gHbClkCnt = 0;//��ʼ��ʱ�Ӽ���������һ��CLK������
	}
	gHbPortRecvData[0] = 0;//��ղ�������
	gHbSampleCnt = 0;//������ݲ�������ֵ
	gHbDatSampleFlag = 1;//��λ������־
	if(gHbClkCnt < 64){/*���ƹ�������ʱ�䴰��*/
		LOP_HB_RESET270US;//��λ270us��ʱ��������270us��ʼ�ɼ�����
	}else{				/*���ƹ��ȡ�ӻ�����ʱ�䴰��*/
		LOP_HB_RESET200US;//��λ200us��ʱ��,����200us�����Clk
	}
}
/*!
 * HbPortGpioInit
 * LOP HB�ӿ�GPIO���ų�ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HbPortGpioInit(void)
{
	//����оƬ38��������ΪGPIO����
	DrvGpioSetPinMux(PORTE,PORTE_PIN2,PIN38_GPIO_PTE2);//PTE2 DA_B_OUT
	//����Ϊ���ģʽ
	DrvGpioSetDirection(GPIOE,PORTE_PIN2,GPIO_DIR_OUTPUT); 	
	LOP_HBDA_OUTL;
	
	DrvGpioSetPinMux(PORTE,PORTE_PIN1,PIN39_GPIO_PTE1);//PTE1 DA_B
	DrvGpioSetDirection(GPIOE,PORTE_PIN1,GPIO_DIR_INPUT);//����Ϊ����
	
	DrvGpioSetPinMux(PORTE,PORTE_PIN0,PIN40_GPIO_PTE0);//PTE0 CLK_B	
	DrvGpioSetDirection(GPIOE,PORTE_PIN0,GPIO_DIR_INPUT);//����Ϊ����
	DrvGpioSetInterrupt(PORTE,PORTE_PIN0,TYPE_RISING_INTERRUPT);//����Ϊ�������ж�
	NVIC_SetPriority(PORTAE_IRQn,0);
	NVIC_EnableIRQ(PORTAE_IRQn);//Enable irq	
}
/*!
 * HbPortTimerInit
 * LOP HB�ӿڶ�ʱ����ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HbPortTimerInit(void)
{
	 /* Ungate the FTM clock*/
	PeripheralClockEnable(PCC_FLEXTMR1_INDEX);
	FTM1->MODE = FTM_MODE_WPDIS_MASK;
	FTM1->SC = 0;
	FTM1->SC |= FTM_SC_PS(5)|FTM_SC_TOIE(1);//48M/32 = 1.5M(each 15 equal 10 us(1/1500000*15 = 0.00001S))
	FTM1->CONF = 0;
	FTM1->MOD = 15000;
	FTM1->CNTIN = HB_TIMER_10MS_VALUE;
	FTM1->CNT = 0; 
	NVIC_SetPriority(FTM1_IRQn,0);
	NVIC_EnableIRQ(FTM1_IRQn);
	LOP_HB_TIMERSTOP;
}
