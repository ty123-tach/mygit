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
//* @{引用头文件声明区域(仅限本文件内使用)
#include "OtisHpiRsl.h"
#include "DrvGpio.h"
#include "MKE16Z4.h"
#include "System.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
/*平台相关定时器值*/
#define HPI_TIMER_100US_VALUE	150		//100us定时器值
#define HPI_TIMER_150US_VALUE	225		//150us定时器值
#define HPI_TIMER_200US_VALUE	300		//200us定时器值
#define HPI_TIMER_270US_VALUE	405		//270us定时器值
#define HPI_TIMER_1P2MS_VALUE	1800	//1.2ms定时器值
#define HPI_TIMER_10MS_VALUE	15000	//10ms定时器值

//**************************************HPI RAM资源***********************************************//
/*	平台相关的RSEB定时器停止 */
#define HPI_TIMERSTOP	(LPTMR0->CSR &= 0xFFFFFFFE)
/*	平台相关的HPI定时器复位100us*/
#define HPI_RESET100US	LPTMR0->CSR &= 0xFFFFFFFE; \
						LPTMR0->CMR  = HPI_TIMER_100US_VALUE; \
						LPTMR0->CSR |= LPTMR_CSR_TEN_MASK
/*	平台相关的HPI定时器复位270us*/
#define HPI_RESET270US	LPTMR0->CSR &= 0xFFFFFFFE; \
						LPTMR0->CMR  = HPI_TIMER_270US_VALUE; \
						LPTMR0->CSR |= LPTMR_CSR_TEN_MASK
/*	平台相关的HPI定时器复位200us*/
#define HPI_RESET200US	LPTMR0->CSR &= 0xFFFFFFFE; \
						LPTMR0->CMR  = HPI_TIMER_200US_VALUE; \
						LPTMR0->CSR |= LPTMR_CSR_TEN_MASK
/*	平台相关的HPI定时器复位10Ms*/
#define HPI_RESET10MS	LPTMR0->CSR &= 0xFFFFFFFE; \
						LPTMR0->CMR  = HPI_TIMER_10MS_VALUE; \
						LPTMR0->CSR |= LPTMR_CSR_TEN_MASK
/*	平台相关的HPI定时器读取TC值*/
#define HPI_READTC		(LPTMR0->CNR)
#define HPI_PREREAD		(LPTMR0->CNR = 0)	/*LPTMR0读取TC值之前需要预写操作*/
									
/*	平台相关的GPIO读取DA_A引脚的输入电平状态*/
#define HPI_DA_READ		(GPIOD->PDIR&GPIOPORT_PIN1_MASK)	//读取DA总线上GPIO口数据
/*	平台相关的GPIO输出DA_A_OUT线上低电平*/
#define HPI_DA_OUTL		GPIOC->PSOR = 0x20;					//HPI协议数据总线拉高 芯片GPIO拉低对应总线拉低
/*	平台相关的GPIO输出DA_A_OUT线上高电平*/
#define HPI_DA_OUTH		GPIOC->PCOR = 0x20;					//HPI协议数据总线拉高 芯片GPIO拉高对应总线拉高 


volatile unsigned char gIsHpiClkSync;//时钟同步状态
volatile unsigned char gHpiClkCnt;//HPI协议时钟个数
volatile unsigned char gHpiSampleDat;//HPI采样的数据
volatile unsigned char gHpiSampleCnt;//HPI数据采样个数
volatile unsigned char gHpiDatSampleFlag;//HPI数据采样标志
volatile unsigned char gHpiTimeOut = 0;//HPI超时
volatile unsigned char gHpiReceiveIndex;//HPI接收数据缓冲区索引
volatile unsigned char gHpiReceiveFinished;
static unsigned char gHpiReceiveBuf[2][64];//HPI数据缓冲区 
static unsigned char *ptHpiReceive;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void HpiPortTimerInit(void);
void HpiPortGpioInit(void);
void HpiPortRamInit(void);
__STATIC_INLINE void HpiPortRisingHandle(void);
__STATIC_INLINE void HpiPortTimerHandle(void); 
//**
// * @}

/*!
 * OtisHpiRslInit
 * HPI接口初始化
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
 * 获取HPI接收数据
 * @param[in]   None
 * @param[out]  None
 * @retval  	返回接收到的数据
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
 * 获取HPI接口超时状态
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
 * 清空HPI接收缓冲区
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
 * HPI接口RAN数据初始化
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
 * HPI定时器中断处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
__STATIC_INLINE void HpiPortTimerHandle(void)
{
	if(gIsHpiClkSync == 0){//时钟等待同步超时
		HPI_TIMERSTOP;
		gHpiTimeOut = 1;
		return;
	}
	if(gHpiDatSampleFlag == 0){//采样结束超时
		HPI_TIMERSTOP;
		gHpiTimeOut = 1;
		return;
	}
	gHpiTimeOut = 0;	
	if(gHpiClkCnt < 64){/*控制柜发送时间窗口*/
		if(gHpiSampleCnt == 0){//CLK上升沿时钟到达后启动定时器后的第一个采样点
			HPI_RESET100US;//更改定时器100us采样一次
		}		
		if(HPI_DA_READ){//DA数据线为高
			gHpiSampleDat |= 1<<gHpiSampleCnt;
		}
		gHpiSampleCnt++;//数据采样点计数
		if(gHpiSampleCnt < 5)//采样次数未结束
			return;
		gHpiReceiveBuf[gHpiReceiveIndex][gHpiClkCnt] = gHpiSampleDat;//保存采样数据
		gHpiClkCnt++;
		if(gHpiClkCnt >= 64){//接收完最后一帧主机写入数据
			ptHpiReceive = gHpiReceiveBuf[gHpiReceiveIndex];
			gHpiReceiveFinished = 1;
			gHpiReceiveIndex ^= 1;//切换接收缓冲区
		}		
	}else{				/*控制柜读取从机数据时间窗口*/
		gHpiClkCnt++;
		if(gHpiClkCnt >= 128){//时钟个数达到协议定义的128帧
			gIsHpiClkSync = 0;//置为等待下一周期同步
		}		
	}
	gHpiDatSampleFlag = 0;
	HPI_RESET10MS;	
}
/*!
 * HpiPortRisingHandle
 * HPI CLK上升沿中断处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
__STATIC_INLINE void HpiPortRisingHandle(void)
{
	if(gHpiDatSampleFlag)//数据采样中,在实测过程中发现,RSL总线的CLK到了MCU端
		return;			//引脚时，下降沿时会出现尖峰抖动，导致MCU会再进入一次中断
	if(gIsHpiClkSync == 0){//时钟等待同步
		HPI_PREREAD;
		//当前TC差值小于1.2ms
		if(HPI_READTC < HPI_TIMER_1P2MS_VALUE){//协议同步时间为1.6ms,帧时间间隔为0.8ms,取1.2ms作为中心点
			HPI_RESET10MS;//复位定时器
			return;//同步时间未到,返回				
		}
		gIsHpiClkSync = 1;//时钟已同步
		gHpiClkCnt = 0;//初始化时钟计数，即第一个CLK上升沿
	}
	gHpiSampleDat = 0;//清空采样数据
	gHpiSampleCnt = 0;//清空数据采样计数值
	gHpiDatSampleFlag = 1;//置位采样标志
//	if(gHpiClkCnt < 64){/*控制柜发送数据时间窗口*/
		HPI_RESET270US;//复位270us定时器，空闲270us后开始采集数据
//	}else{				/*控制柜读取从机数据时间窗口*/
//		HPI_RESET200US;//复位200us定时器,空闲200us后计数Clk
//	}	
}
/*!
 * LPIT0_IRQHandler
 * 平台相关定时器中断
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
 * 平台相关RSEB CLK上升沿中断
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
 * HPI定时器初始化
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
	LPTMR0->CMR = HPI_TIMER_10MS_VALUE;//10ms中断 
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
 * HPI接口GPIO初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void HpiPortGpioInit(void)
{
    //芯片物理41号引脚作为GPIO功能
	DrvGpioSetPinMux(PORTC,PORTC_PIN5,PIN41_GPIO_PTC5);//PTC5 DA_A_OUT
	//设置为输出模式
	DrvGpioSetDirection(GPIOC,PORTC_PIN5,GPIO_DIR_OUTPUT); 		
	HPI_DA_OUTL;//拉低总线
	
	DrvGpioSetPinMux(PORTD,PORTD_PIN1,PIN1_GPIO_PTD1);//PTD1 DA_A
	DrvGpioSetDirection(GPIOD,PORTD_PIN1,GPIO_DIR_INPUT);//配置为输入
	
	DrvGpioSetPinMux(PORTD,PORTD_PIN0,PIN2_GPIO_PTD0);//PTD0 CLK_A
	DrvGpioSetDirection(GPIOD,PORTD_PIN0,GPIO_DIR_INPUT);//配置为输入
	DrvGpioSetInterrupt(PORTD,PORTD_PIN0,TYPE_RISING_INTERRUPT);//配置为上升沿中断
	NVIC_SetPriority(PORTBCD_IRQn,0);
	NVIC_EnableIRQ(PORTBCD_IRQn);//Enable irq
}

