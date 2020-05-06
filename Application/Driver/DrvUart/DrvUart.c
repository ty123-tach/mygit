/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: DrvUart.c
	Author: Zhang zhiwei Version: V0 Date: 03. July. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "DrvUart.h"
#include "DrvGpio.h"
#include "System.h"
#include "MKE16Z4.h"
#include "ZkPtlLowlevel.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#define USE_UART	1
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)

//**
// * @}
/*!
 * setModuloBaudRate
 * set module baudrate
 * 设定模块波特率
 * @param[in]   base：串口模块外设基址
				baud：波特率值
 * @param[out]  None
 * @retval  	0：成功	1：失败
 */
unsigned char setModuloBaudRate(LPUART_Type *base,unsigned int baud)
{
	/* BaudRate = LPUART ASYNCH Module Clock/SBR[12:0]*(OSR+1) */
	unsigned int lReg = base->BAUD;
	unsigned int lOsr = 0,lOsrTemp = 0;
	unsigned int lCalculateBaud = 0;
	unsigned int lCalDiff = 0,lBaudDiff = 0;
	unsigned int lSysClk = getAsyncClock();
	unsigned short int lSbr,lSbrTemp;
	
	lBaudDiff = baud;
	lOsr = 0;
	lSbr = 0;
	for(lOsrTemp = 4; lOsrTemp <= 32; lOsrTemp++){
		lSbrTemp = lSysClk/(baud*lOsrTemp);
		if(lSbrTemp == 0)
			lSbrTemp = 1;
		lCalculateBaud = lSysClk/(lSbrTemp*lOsrTemp);
		lCalDiff = lCalculateBaud - baud;
		/* Select the better value between sbr and (sbr + 1) */
		if(lCalDiff > (baud - (lSysClk/(lOsrTemp*(lSbrTemp+1))))){
			lCalDiff = baud - (lSysClk/(lOsrTemp*(lSbrTemp+1)));
			lSbrTemp++;
		}
		if(lCalDiff <= lBaudDiff){
			lBaudDiff = lCalDiff;
            lOsr = lOsrTemp; /* update and store the best OSR value calculated */
            lSbr = lSbrTemp; /* update store the best SBR value calculated */			
		}
	}
    /* Check to see if actual baud rate is within 3% of desired baud rate
     * based on the best calculate OSR value */	
	if(lBaudDiff > (baud/100)*3){
		return 1;
	}
    /* Acceptable baud rate, check if OSR is between 4x and 7x oversampling.
     * If so, then "BOTHEDGE" sampling must be turned on */
    if((lOsr > 3) && (lOsr < 8)){
        lReg |= LPUART_BAUD_BOTHEDGE_MASK;
    }	
	/* program the osr value (bit value is one less than actual value) */
    lReg &= ~LPUART_BAUD_OSR_MASK;
    lReg |= LPUART_BAUD_OSR(lOsr - 1);
	/* write the sbr value to the BAUD registers */
	lReg &= ~LPUART_BAUD_SBR_MASK;
	lReg |= LPUART_BAUD_SBR(lSbr);
	/* receiver and transmitter use 7bit to 9bit data characters. */
	lReg &= ~LPUART_BAUD_M10_MASK;
	/*one stop bit*/
	lReg &= ~LPUART_BAUD_SBNS_MASK;
	base->BAUD = lReg;
	return 0;
}
#if USE_UART
/*!
 * DrvUartInit
 * lpuart init
 * 串口初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvUartInit(void)
{
	unsigned int lReg;

	/*gpio periphearl clock*/
	PeripheralClockEnable(PCC_PORTC_INDEX);
	/*PORTC6 (pin 37)is configured as LPUART1_RX*/
	DrvGpioSetPinMux(PORTC,6,PIN37_LPUART1_RX);
	/*PORTC7 (pin 36is configured as LPUART1_TX*/
	DrvGpioSetPinMux(PORTC,7,PIN36_LPUART1_TX);

	/*select clock source*/
	PeripheralClockSrcSelect(PCC_LPUART1_INDEX,CLK_FIRC_SRC);
	/*Reset all internal logic and registers, except the Global Register */
	LPUART1->GLOBAL |= LPUART_GLOBAL_RST_MASK;
	LPUART1->GLOBAL &= ~LPUART_GLOBAL_RST_MASK;
	setModuloBaudRate(LPUART1,9600);
	lReg = LPUART1->CTRL&~(LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK | LPUART_CTRL_M_MASK | LPUART_CTRL_ILT_MASK |
             LPUART_CTRL_IDLECFG_MASK);
	/* receiver and transmitter use 8bit to 10 bit data character*/
	lReg &= ~LPUART_CTRL_M7_MASK;
	/* receiver and transmitter use 8bit data characters*/
	LPUART1->CTRL = lReg;
	/* Clear all status flags */
    lReg = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
            LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);	
	lReg &= ~LPUART_STAT_MSBF_MASK;
	LPUART1->STAT |= lReg;
	LPUART1->CTRL |= LPUART_CTRL_TE_MASK|LPUART_CTRL_RE_MASK|LPUART_CTRL_RIE_MASK;
	//使用BST协议时需要使能下列内容
	LPUART1->CTRL |= LPUART_CTRL_M_MASK;
	LPUART1->CTRL |= LPUART_CTRL_PE(1)|LPUART_CTRL_PT(0);//使能奇偶校验为偶校验
	/*enable system interrupt*/
    NVIC_SetPriority(LPUART1_IRQn,2);
	NVIC_EnableIRQ(LPUART1_IRQn);		
}
/*!
 * DrvUartDeinit
 * 串口复位初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvUartDeinit(void)
{
	uint32_t temp;
	LPUART_Type *ptUart;
	if((PCC->CLKCFG[PCC_LPUART1_INDEX] & PCC_CLKCFG_CGC_MASK) == 0)
		return;	
	ptUart = LPUART1;
    /* Wait last char shift out */
    while(0 == (ptUart->STAT & LPUART_STAT_TC_MASK)){
    }
    /* Clear all status flags */
    temp = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
            LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);	
    ptUart->STAT |= temp;
    /* Disable the module. */
    ptUart->CTRL = 0;	
	ptUart->GLOBAL = LPUART_GLOBAL_RST(1);
	ptUart->GLOBAL = LPUART_GLOBAL_RST(0);
	NVIC_DisableIRQ(LPUART1_IRQn);
	/* Disable lpuart clock */
	PCC->CLKCFG[PCC_LPUART1_INDEX] &= ~PCC_CLKCFG_CGC_MASK;
}
/*!
 * ZkPtlHwSendData
 * ZK协议硬件发送数据
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlHwSendData(void *dat,unsigned long size)
{
	unsigned char *lAdr = (unsigned char *)dat;
	unsigned long len = size;
	while(len--){
		while((LPUART1->STAT&LPUART_STAT_TDRE_MASK) == 0);
		LPUART1->DATA = *lAdr++;
	}
}
/*!
 * DrvUart1SenCh
 * 串口发送字符
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvUart1SenCh(unsigned char ch)
{
	while((LPUART1->STAT&LPUART_STAT_TDRE_MASK) == 0);
	LPUART1->DATA = ch;
}
/*!
 * LPUART1_IRQHandler
 * 串口1中断
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LPUART1_IRQHandler(void)
{
	ZkPtlLowlevelRecCh(LPUART1->DATA);
}
#else
/*!
 * DrvUartInit
 * lpuart init
 * 串口初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvUartInit(void)
{
	unsigned int lReg;

	/*gpio periphearl clock*/
	PeripheralClockEnable(PCC_PORTD_INDEX);
	/*PORTD7 (pin 16)is configured as LPUART2_TX*/
	DrvGpioSetPinMux(PORTD,6,PIN16_LPUART2_TX);
	/*PORTD6 (pin 17)is configured as LPUART2_RX*/
	DrvGpioSetPinMux(PORTD,7,PIN17_LPUART2_RX);

	/*select clock source*/
	PeripheralClockSrcSelect(PCC_LPUART2_INDEX,CLK_FIRC_SRC);
	/*Reset all internal logic and registers, except the Global Register */
	LPUART2->GLOBAL |= LPUART_GLOBAL_RST_MASK;
	LPUART2->GLOBAL &= ~LPUART_GLOBAL_RST_MASK;
	setModuloBaudRate(LPUART2,115200);
	lReg = LPUART2->CTRL&~(LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK | LPUART_CTRL_M_MASK | LPUART_CTRL_ILT_MASK |
             LPUART_CTRL_IDLECFG_MASK);
	/* receiver and transmitter use 8bit to 10 bit data character*/
	lReg &= ~LPUART_CTRL_M7_MASK;
	/* receiver and transmitter use 8bit data characters*/
	LPUART2->CTRL = lReg;
	/* Clear all status flags */
    lReg = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
            LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);	
	lReg &= ~LPUART_STAT_MSBF_MASK;
	LPUART2->STAT |= lReg;
	LPUART2->CTRL |= LPUART_CTRL_TE_MASK|LPUART_CTRL_RE_MASK|LPUART_CTRL_RIE_MASK;
	/*enable system interrupt*/
    NVIC_SetPriority(LPUART2_IRQn,0);
	NVIC_EnableIRQ(LPUART2_IRQn);		
}
/*!
 * DrvUartDeinit
 * 串口复位初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvUartDeinit(void)
{
	uint32_t temp;

	if((PCC->CLKCFG[PCC_LPUART2_INDEX] & PCC_CLKCFG_CGC_MASK) == 0)
		return;

    /* Wait last char shift out */
    while(0 == (LPUART2->STAT & LPUART_STAT_TC_MASK)){
    }
    /* Clear all status flags */
    temp = (LPUART_STAT_RXEDGIF_MASK | LPUART_STAT_IDLE_MASK | LPUART_STAT_OR_MASK | LPUART_STAT_NF_MASK |
            LPUART_STAT_FE_MASK | LPUART_STAT_PF_MASK);	
    LPUART2->STAT |= temp;
    /* Disable the module. */
    LPUART2->CTRL = 0;	
	LPUART2->GLOBAL = LPUART_GLOBAL_RST(1);
	LPUART2->GLOBAL = LPUART_GLOBAL_RST(0);	
	NVIC_DisableIRQ(LPUART2_IRQn);
	/* Disable lpuart clock */
	PCC->CLKCFG[PCC_LPUART2_INDEX] &= ~PCC_CLKCFG_CGC_MASK;
}
/*!
 * ZkPtlHwSendData
 * ZK协议硬件发送数据
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void ZkPtlHwSendData(void *dat,unsigned long size)
{
	unsigned char *lAdr = (unsigned char *)dat;
	unsigned long len = size;
	while(len--){
		while((LPUART2->STAT&LPUART_STAT_TDRE_MASK) == 0);
		LPUART2->DATA = *lAdr++;
	}
}
/*!
 * LPUART1_IRQHandler
 * 串口1中断
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void LPUART2_IRQHandler(void)
{
	ZkPtlLowlevelRecCh(LPUART2->DATA);
}

#endif
