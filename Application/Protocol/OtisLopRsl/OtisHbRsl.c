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
//* @{引用头文件声明区域(仅限本文件内使用)
#include "OtisHbRsl.h"
#include "System.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
/*平台相关HB定时器值*/
#define HB_TIMER_100US_VALUE	14850	//100us定时器值
#define HB_TIMER_200US_VALUE	14700	//200us定时器值
#define HB_TIMER_270US_VALUE	14595	//270us定时器值
#define HB_TIMER_1P2MS_VALUE	1800	//1.2ms定时器值
#define HB_TIMER_10MS_VALUE		0		//10ms定时器值
/*	平台相关的HB定时器停止*/
#define LOP_HB_TIMERSTOP 	(FTM1->SC &= 0xFFFFFFE7)
/*	平台相关的HB定时器复位100us*/
#define LOP_HB_RESET100US	FTM1->SC &= 0xFFFFFFE7; \
							FTM1->CNTIN = HB_TIMER_100US_VALUE;	\
							FTM1->CNT = 0;			\
							FTM1->SC |= 0x08
/*	平台相关的HB定时器复位200us*/
#define LOP_HB_RESET200US	FTM1->SC &= 0xFFFFFFE7; \
							FTM1->CNTIN = HB_TIMER_200US_VALUE;	\
							FTM1->CNT = 0;			\
							FTM1->SC |= 0x08
/*	平台相关的HB定时器复位270us*/
#define LOP_HB_RESET270US	FTM1->SC &= 0xFFFFFFE7; \
							FTM1->CNTIN = HB_TIMER_270US_VALUE;	\
							FTM1->CNT = 0;			\
							FTM1->SC |= 0x08
/*	平台相关的HB定时器复位10ms*/
#define LOP_HB_RESET10MS	FTM1->SC &= 0xFFFFFFE7; \
							FTM1->CNTIN = HB_TIMER_10MS_VALUE;	\
							FTM1->CNT = 0;			\
							FTM1->SC |= 0x08							
/*	平台相关的HPI定时器读取TC值*/
#define LOP_HB_READTC		(FTM1->CNT)

/*	平台相关的GPIO输出DA_B_OUT线上高电平*/
#define LOP_HBDA_OUTH 	GPIOE->PCOR = 0x04	//芯片GPIO口拉低,数据总线拉高
/*	平台相关的GPIO输出DA_B_OUT线上低电平*/
#define LOP_HBDA_OUTL	GPIOE->PSOR = 0x04	//芯片GPIO口拉高,数据总线拉低
/*	平台相关的GPIO读取DA_A引脚的输入电平状态*/
#define PLATFORM_HBDA_READ	(GPIOE->PDIR&GPIOPORT_PIN1_MASK)	//读取DA总线上GPIO口数据	
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void HbPortTimerInit(void);
void HbPortGpioInit(void);
void HbPortRamInit(void);
__STATIC_INLINE void HbPortRisingHandle(void);
__STATIC_INLINE void HbPortTimerHandle(void);
//**
// * @}
volatile unsigned char gHbDatSampleFlag;//HB接口数据采样标志
volatile unsigned char gHbSampleCnt;//HB接口数据采样次数
volatile unsigned char gIsHbClkSync;//时钟同步状态
volatile unsigned char gHbClkCnt;//HB接口时钟个数
volatile unsigned char gHbTimeOut = 0;//HB接口超时
volatile unsigned char gHbPortReadAdress;//HB接口读取地址
volatile unsigned char gHbPortRecvData[2];//HB接口接收的数据
volatile unsigned char gHbPortWriteAdress;//HB接口写数据地址
volatile unsigned char gHbPortSendData[2];//HB接口发送的数据
volatile unsigned char gHbLockAdress = 0;//HB接口锁梯地址
volatile unsigned char gHbLockSendData[2];//HB接口锁梯发送数据
/*!
 * OtisHbRslInit
 * HB接口初始化
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
 * 获取HB接口超时状态
 * @param[in]   None
 * @param[out]  None
 * @retval  	1：超时	0：无超时
 */
unsigned char GetHbPortTimeOutSta(void)
{
	return gHbTimeOut;
}
/*!
 * GetHbPortReceiveData
 * 获取HB接口接收的数据
 * @param[in]   None
 * @param[out]  None
 * @retval  	接收到的数据
 */
unsigned char GetHbPortReceiveData(void)
{
	return gHbPortRecvData[1];
}
/*!
 * SetHbPortSendData
 * 设置HB接口发送数据
 * @param[in]   data：待发送的数据
 * @param[out]  None
 * @retval  	None
 */
void SetHbPortSendData(unsigned char data)
{
	gHbPortSendData[1] = data;
}
/*!
 * SetHbPortAddress
 * 设置HB接口地址
 * @param[in]   adr：接口地址值
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
 * 设置HB锁梯数据
 * @param[in]   data：待发送的数据
 * @param[out]  None
 * @retval  	None
 */
void SetHbLockSendData(unsigned char data)
{
	gHbLockSendData[1] = data;
}
/*!
 * SetHbLockAddress
 * 设置HB锁梯地址
 * @param[in]   adr：地址值
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
 * HB接口RAM数据初始化
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
 * LOP HB 定时器中断
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
 * LOP HB CLK上升沿中断
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
 * LOP HB接口定时器中断处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
__STATIC_INLINE void HbPortTimerHandle(void)
{
	if(gIsHbClkSync == 0){//时钟等待同步超时
		LOP_HB_TIMERSTOP;
		gHbTimeOut = 1;
		return;
	}
	if(gHbDatSampleFlag == 0){//采样结束超时
		LOP_HB_TIMERSTOP;
		gHbTimeOut = 1;
		return;
	}
	gHbTimeOut = 0;	
	if(gHbClkCnt < 64){/*控制柜发送时间窗口*/
		if(gHbSampleCnt == 0){//CLK上升沿时钟到达后启动定时器后的第一个采样点
			LOP_HB_RESET100US;//更改定时器100us采样一次
		}		
		if(PLATFORM_HBDA_READ){//DA数据线为高
			gHbPortRecvData[0] |= 1<<gHbSampleCnt;
		}
		gHbSampleCnt++;//数据采样点计数
		if(gHbSampleCnt < 5)//采样次数未结束
			return;
		if(gHbPortReadAdress == gHbClkCnt){//读地址匹配
			if((gHbPortRecvData[0]&0x0F) != gHbPortRecvData[1])
				gHbPortRecvData[1] = gHbPortRecvData[0]&0x0F;//保存采样数据
		}
		gHbClkCnt++;		
	}else{				/*控制柜读取从机数据时间窗口*/
		if((gHbPortWriteAdress == gHbClkCnt)||(gHbLockAdress == gHbClkCnt)){//写地址匹配
			if(gHbSampleCnt < 5){//采样次数未结束
				if(gHbSampleCnt == 0){//CLK上升沿时钟到达后启动定时器后的第一个采样点
					if(gHbPortWriteAdress == gHbClkCnt){
						if(gHbPortSendData[0] != gHbPortSendData[1]){
							gHbPortSendData[0] = gHbPortSendData[1];
						}				
					}else{
						if(gHbLockSendData[0] != gHbLockSendData[1]){
							gHbLockSendData[0] = gHbLockSendData[1];
						}						
					}
					LOP_HB_RESET100US;//更改定时器100us采样一次
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
				gHbSampleCnt++;//数据采样点计数
				return;
			}		
		}
		LOP_HBDA_OUTL;
		gHbClkCnt++;
		if(gHbClkCnt >= 128){//时钟个数达到协议定义的128帧
			gIsHbClkSync = 0;//置为等待下一周期同步
		}		
	}
	gHbDatSampleFlag = 0;
	LOP_HB_RESET10MS;
}
/*!
 * HbPortRisingHandle
 * LOP HB接口上升沿中断处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
__STATIC_INLINE void HbPortRisingHandle(void)
{
	if(gHbDatSampleFlag)//数据采样中,在实测过程中发现,RSL总线的CLK到了MCU端
		return;			//引脚时，下降沿时会出现尖峰抖动，导致MCU会再进入一次中断
	if(gIsHbClkSync == 0){//时钟等待同步
		//当前TC差值小于1.2ms
		if(LOP_HB_READTC < HB_TIMER_1P2MS_VALUE){//协议同步时间为1.6ms,帧时间间隔为0.8ms,取1.2ms作为中心点
			LOP_HB_RESET10MS;//复位定时器			
			return;//同步时间未到,返回				
		}
		gIsHbClkSync = 1;//时钟已同步
		gHbClkCnt = 0;//初始化时钟计数，即第一个CLK上升沿
	}
	gHbPortRecvData[0] = 0;//清空采样数据
	gHbSampleCnt = 0;//清空数据采样计数值
	gHbDatSampleFlag = 1;//置位采样标志
	if(gHbClkCnt < 64){/*控制柜发送数据时间窗口*/
		LOP_HB_RESET270US;//复位270us定时器，空闲270us后开始采集数据
	}else{				/*控制柜读取从机数据时间窗口*/
		LOP_HB_RESET200US;//复位200us定时器,空闲200us后计数Clk
	}
}
/*!
 * HbPortGpioInit
 * LOP HB接口GPIO引脚初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void HbPortGpioInit(void)
{
	//物理芯片38号引脚作为GPIO功能
	DrvGpioSetPinMux(PORTE,PORTE_PIN2,PIN38_GPIO_PTE2);//PTE2 DA_B_OUT
	//设置为输出模式
	DrvGpioSetDirection(GPIOE,PORTE_PIN2,GPIO_DIR_OUTPUT); 	
	LOP_HBDA_OUTL;
	
	DrvGpioSetPinMux(PORTE,PORTE_PIN1,PIN39_GPIO_PTE1);//PTE1 DA_B
	DrvGpioSetDirection(GPIOE,PORTE_PIN1,GPIO_DIR_INPUT);//配置为输入
	
	DrvGpioSetPinMux(PORTE,PORTE_PIN0,PIN40_GPIO_PTE0);//PTE0 CLK_B	
	DrvGpioSetDirection(GPIOE,PORTE_PIN0,GPIO_DIR_INPUT);//配置为输入
	DrvGpioSetInterrupt(PORTE,PORTE_PIN0,TYPE_RISING_INTERRUPT);//配置为上升沿中断
	NVIC_SetPriority(PORTAE_IRQn,0);
	NVIC_EnableIRQ(PORTAE_IRQn);//Enable irq	
}
/*!
 * HbPortTimerInit
 * LOP HB接口定时器初始化
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
