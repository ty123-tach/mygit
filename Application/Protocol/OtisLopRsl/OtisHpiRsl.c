/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: OtisHpiRsl.c
	Author: ZhiweiZhang Version: V0 Date: 2019-10-24
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "OtisHpiRsl.h"
#include "DrvGpio.h"
#include "MKE16Z4.h"
#include "System.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
/*ƽ̨��ض�ʱ��ֵ*/
#define HPI_TIMER_100US_VALUE	150		//100us��ʱ��ֵ
#define HPI_TIMER_150US_VALUE	225		//150us��ʱ��ֵ
#define HPI_TIMER_200US_VALUE	300		//200us��ʱ��ֵ
#define HPI_TIMER_270US_VALUE	405		//270us��ʱ��ֵ
#define HPI_TIMER_1P2MS_VALUE	1800	//1.2ms��ʱ��ֵ
#define HPI_TIMER_10MS_VALUE	15000	//10ms��ʱ��ֵ

//**************************************HPI RAM��Դ***********************************************//
/*	ƽ̨��ص�RSEB��ʱ��ֹͣ */
#define HPI_TIMERSTOP	(LPTMR0->CSR &= 0xFFFFFFFE)
/*	ƽ̨��ص�HPI��ʱ����λ100us*/
#define HPI_RESET100US	LPTMR0->CSR &= 0xFFFFFFFE; \
						LPTMR0->CMR  = HPI_TIMER_100US_VALUE; \
						LPTMR0->CSR |= LPTMR_CSR_TEN_MASK
/*	ƽ̨��ص�HPI��ʱ����λ270us*/
#define HPI_RESET270US	LPTMR0->CSR &= 0xFFFFFFFE; \
						LPTMR0->CMR  = HPI_TIMER_270US_VALUE; \
						LPTMR0->CSR |= LPTMR_CSR_TEN_MASK
/*	ƽ̨��ص�HPI��ʱ����λ200us*/
#define HPI_RESET200US	LPTMR0->CSR &= 0xFFFFFFFE; \
						LPTMR0->CMR  = HPI_TIMER_200US_VALUE; \
						LPTMR0->CSR |= LPTMR_CSR_TEN_MASK
/*	ƽ̨��ص�HPI��ʱ����λ10Ms*/
#define HPI_RESET10MS	LPTMR0->CSR &= 0xFFFFFFFE; \
						LPTMR0->CMR  = HPI_TIMER_10MS_VALUE; \
						LPTMR0->CSR |= LPTMR_CSR_TEN_MASK
/*	ƽ̨��ص�HPI��ʱ����ȡTCֵ*/
#define HPI_READTC		(LPTMR0->CNR)
#define HPI_PREREAD		(LPTMR0->CNR = 0)	/*LPTMR0��ȡTCֵ֮ǰ��ҪԤд����*/
									
/*	ƽ̨��ص�GPIO��ȡDA_A���ŵ������ƽ״̬*/
#define HPI_DA_READ		(GPIOD->PDIR&GPIOPORT_PIN1_MASK)	//��ȡDA������GPIO������
/*	ƽ̨��ص�GPIO���DA_A_OUT���ϵ͵�ƽ*/
#define HPI_DA_OUTL		GPIOC->PSOR = 0x20;					//HPIЭ�������������� оƬGPIO���Ͷ�Ӧ��������
/*	ƽ̨��ص�GPIO���DA_A_OUT���ϸߵ�ƽ*/
#define HPI_DA_OUTH		GPIOC->PCOR = 0x20;					//HPIЭ�������������� оƬGPIO���߶�Ӧ�������� 


volatile unsigned char gIsHpiClkSync;//ʱ��ͬ��״̬
volatile unsigned char gHpiClkCnt;//HPIЭ��ʱ�Ӹ���
volatile unsigned char gHpiSampleDat;//HPI����������
volatile unsigned char gHpiSampleCnt;//HPI���ݲ�������
volatile unsigned char gHpiDatSampleFlag;//HPI���ݲ�����־
volatile unsigned char gHpiTimeOut = 0;//HPI��ʱ
volatile unsigned char gHpiReceiveIndex;//HPI�������ݻ���������
volatile unsigned char gHpiReceiveFinished;
static unsigned char gHpiReceiveBuf[2][64];//HPI���ݻ����� 
static unsigned char *ptHpiReceive;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void HpiPortTimerInit(void);
void HpiPortGpioInit(void);
void HpiPortRamInit(void);
__STATIC_INLINE void HpiPortRisingHandle(void);
__STATIC_INLINE void HpiPortTimerHandle(void); 
//**
// * @}

/*!
 * OtisHpiRslInit
 * HPI�ӿڳ�ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void OtisHpiRslInit(void)
{
	HpiPortTimerInit();
	HpiPortGpioInit();
	HpiPortRamInit();
}
/*!
 * GetHpiReceiveData
 * ��ȡHPI��������
 * @param[in]   None
 * @param[out]  None
 * @retval  	���ؽ��յ�������
 */
unsigned char *GetHpiReceiveData(void)
{
	if(gHpiReceiveFinished == 0)
		return 0;
	gHpiReceiveFinished = 0;
	return ptHpiReceive;
}
/*!
 * GetHpiTimeOut
 * ��ȡHPI�ӿڳ�ʱ״̬
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
unsigned char GetHpiTimeOut(void)
{
	return gHpiTimeOut;
}
/*!
 * HpiReceiveBufferClean
 * ���HPI���ջ�����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HpiReceiveBufferClean(void)
{
	unsigned char lCnt;
	
	for(lCnt = 0; lCnt < 64; lCnt++){
		gHpiReceiveBuf[0][lCnt] =0;
		gHpiReceiveBuf[1][lCnt] =0;
	}
}
/*!
 * HpiPortRamInit
 * HPI�ӿ�RAN���ݳ�ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HpiPortRamInit(void)
{
	gIsHpiClkSync = 0;
	gHpiClkCnt = 0;
	gHpiSampleDat = 0;
	gHpiSampleCnt = 0;
	gHpiDatSampleFlag = 0;
	gHpiTimeOut = 0;
	gHpiReceiveIndex = 0;
	ptHpiReceive = gHpiReceiveBuf[gHpiReceiveIndex];	
}
/*!
 * HpiPortTimerHandle
 * HPI��ʱ���жϴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
__STATIC_INLINE void HpiPortTimerHandle(void)
{
	if(gIsHpiClkSync == 0){//ʱ�ӵȴ�ͬ����ʱ
		HPI_TIMERSTOP;
		gHpiTimeOut = 1;
		return;
	}
	if(gHpiDatSampleFlag == 0){//����������ʱ
		HPI_TIMERSTOP;
		gHpiTimeOut = 1;
		return;
	}
	gHpiTimeOut = 0;	
	if(gHpiClkCnt < 64){/*���ƹ���ʱ�䴰��*/
		if(gHpiSampleCnt == 0){//CLK������ʱ�ӵ����������ʱ����ĵ�һ��������
			HPI_RESET100US;//���Ķ�ʱ��100us����һ��
		}		
		if(HPI_DA_READ){//DA������Ϊ��
			gHpiSampleDat |= 1<<gHpiSampleCnt;
		}
		gHpiSampleCnt++;//���ݲ��������
		if(gHpiSampleCnt < 5)//��������δ����
			return;
		gHpiReceiveBuf[gHpiReceiveIndex][gHpiClkCnt] = gHpiSampleDat;//�����������
		gHpiClkCnt++;
		if(gHpiClkCnt >= 64){//���������һ֡����д������
			ptHpiReceive = gHpiReceiveBuf[gHpiReceiveIndex];
			gHpiReceiveFinished = 1;
			gHpiReceiveIndex ^= 1;//�л����ջ�����
		}		
	}else{				/*���ƹ��ȡ�ӻ�����ʱ�䴰��*/
		gHpiClkCnt++;
		if(gHpiClkCnt >= 128){//ʱ�Ӹ����ﵽЭ�鶨���128֡
			gIsHpiClkSync = 0;//��Ϊ�ȴ���һ����ͬ��
		}		
	}
	gHpiDatSampleFlag = 0;
	HPI_RESET10MS;	
}
/*!
 * HpiPortRisingHandle
 * HPI CLK�������жϴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
__STATIC_INLINE void HpiPortRisingHandle(void)
{
	if(gHpiDatSampleFlag)//���ݲ�����,��ʵ������з���,RSL���ߵ�CLK����MCU��
		return;			//����ʱ���½���ʱ����ּ�嶶��������MCU���ٽ���һ���ж�
	if(gIsHpiClkSync == 0){//ʱ�ӵȴ�ͬ��
		HPI_PREREAD;
		//��ǰTC��ֵС��1.2ms
		if(HPI_READTC < HPI_TIMER_1P2MS_VALUE){//Э��ͬ��ʱ��Ϊ1.6ms,֡ʱ����Ϊ0.8ms,ȡ1.2ms��Ϊ���ĵ�
			HPI_RESET10MS;//��λ��ʱ��
			return;//ͬ��ʱ��δ��,����				
		}
		gIsHpiClkSync = 1;//ʱ����ͬ��
		gHpiClkCnt = 0;//��ʼ��ʱ�Ӽ���������һ��CLK������
	}
	gHpiSampleDat = 0;//��ղ�������
	gHpiSampleCnt = 0;//������ݲ�������ֵ
	gHpiDatSampleFlag = 1;//��λ������־
//	if(gHpiClkCnt < 64){/*���ƹ�������ʱ�䴰��*/
		HPI_RESET270US;//��λ270us��ʱ��������270us��ʼ�ɼ�����
//	}else{				/*���ƹ��ȡ�ӻ�����ʱ�䴰��*/
//		HPI_RESET200US;//��λ200us��ʱ��,����200us�����Clk
//	}	
}
/*!
 * LPIT0_IRQHandler
 * ƽ̨��ض�ʱ���ж�
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void PWT_LPTMR0_IRQHandler(void)
{
	LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
	HpiPortTimerHandle();
}
/*!
 * PORTBCD_IRQHandler
 * ƽ̨���RSEB CLK�������ж�
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void PORTBCD_IRQHandler(void)
{
	PORTD->ISFR = GPIOPORT_PIN0_MASK;
	HpiPortRisingHandle();
}
/*!
 * HpiPortTimerInit
 * HPI��ʱ����ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void HpiPortTimerInit(void)
{
	/* Ungate the LPTMR clock*/
	PeripheralClockSrcSelect(PCC_LPTMR0_INDEX,CLK_FIRC_SRC);
	LPTMR0->CSR = 0;
	/*select low power oscillator clock 128K default prescaler clock by 1*/
	LPTMR0->PSR = LPTMR_PSR_PBYP(0)|		//0-enable Prescaler/glitch filter	 1-bypassed
				   LPTMR_PSR_PRESCALE(4)|	//prescale by 32 48M/32 = 1.5M (each 15 equal 10 us(1/1500000*15 = 0.00001S))
				   LPTMR_PSR_PCS(0);		//0-BUS_CLK 1-LPO_CLK 2-RTC_CLKIN 3-X
	LPTMR0->CMR = HPI_TIMER_10MS_VALUE;//10ms�ж� 
	/*enable Timer interrupt*/
	LPTMR0->CSR |= LPTMR_CSR_TIE_MASK;
	/*enable system interrupt*/
    NVIC_SetPriority(PWT_LPTMR0_IRQn,0);
	NVIC_EnableIRQ(PWT_LPTMR0_IRQn);	
	/*stop timer*/
	LPTMR0->CNR = 0;
	LPTMR0->CSR |= LPTMR_CSR_TEN(0);//LPTMR_CSR_TEN_MASK;
	HPI_TIMERSTOP;
}
/*!
 * HpiPortGpioInit
 * HPI�ӿ�GPIO��ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void HpiPortGpioInit(void)
{
    //оƬ����41��������ΪGPIO����
	DrvGpioSetPinMux(PORTC,PORTC_PIN5,PIN41_GPIO_PTC5);//PTC5 DA_A_OUT
	//����Ϊ���ģʽ
	DrvGpioSetDirection(GPIOC,PORTC_PIN5,GPIO_DIR_OUTPUT); 		
	HPI_DA_OUTL;//��������
	
	DrvGpioSetPinMux(PORTD,PORTD_PIN1,PIN1_GPIO_PTD1);//PTD1 DA_A
	DrvGpioSetDirection(GPIOD,PORTD_PIN1,GPIO_DIR_INPUT);//����Ϊ����
	
	DrvGpioSetPinMux(PORTD,PORTD_PIN0,PIN2_GPIO_PTD0);//PTD0 CLK_A
	DrvGpioSetDirection(GPIOD,PORTD_PIN0,GPIO_DIR_INPUT);//����Ϊ����
	DrvGpioSetInterrupt(PORTD,PORTD_PIN0,TYPE_RISING_INTERRUPT);//����Ϊ�������ж�
	NVIC_SetPriority(PORTBCD_IRQn,0);
	NVIC_EnableIRQ(PORTBCD_IRQn);//Enable irq
}

