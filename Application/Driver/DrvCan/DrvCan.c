/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvCan.c
	Author: ZhiweiZhang Version: V0 Date: 2019-12-10
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "DrvCan.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
#include "System.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
/*! @brief MsCAN Rx Message Buffer Mask helper macro. */
#define MSCAN_RX_MB_STD_MASK(id)               \
    ((uint32_t)((((uint32_t)(id)&0x7) << 21) | \
                ((((uint32_t)(id) >> 3) & 0xFF) << 24))) /*!< Standard Rx Message Buffer Mask helper macro. */
#define MSCAN_RX_MB_EXT_MASK(id)                                                                   \
    ((uint32_t)(((((uint32_t)(id) >> 21) & 0xFF) << 24) | ((((uint32_t)(id) >> 18) & 0x7) << 21) | \
                ((((uint32_t)(id) >> 15) & 0x7) << 16) | (((((uint32_t)(id) >> 7) & 0xFF)) << 8) | \
                (((uint32_t)(id)&0x7F) << 1))) /*!< Extend Rx Message Buffer Mask helper macro. */ 
				
#define MSCAN_TIME_QUANTA_NUM (10)

s_USRDATA_T *ptUsr;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void DrvCanWriteIDMR(unsigned char bank,unsigned char *ptID);
void DrvCanWriteIDAR(unsigned char bank,unsigned char *ptID);
void DrvCanEnterInitMode(void);
void DrvCanExitInitMode(void);
//**
// * @}
/*!
 * DrvCanWriteIDAR
 * 写ID标识符接受寄存器
 * @param[in]   bank：0-BANK1	!0-BANK2
				ptID：ID标识符值指针
 * @param[out]  None
 * @retval  	None
 */ 
void DrvCanWriteIDAR(unsigned char bank,unsigned char *ptID)
{
	if(bank){
		MSCAN->CANIDAR_BANK_2[0] = ptID[3];
		MSCAN->CANIDAR_BANK_2[1] = ptID[2];
		MSCAN->CANIDAR_BANK_2[2] = ptID[1];
		MSCAN->CANIDAR_BANK_2[3] = ptID[0];		
	}else{
		MSCAN->CANIDAR_BANK_1[0] = ptID[3];
		MSCAN->CANIDAR_BANK_1[1] = ptID[2];
		MSCAN->CANIDAR_BANK_1[2] = ptID[1];
		MSCAN->CANIDAR_BANK_1[3] = ptID[0];	
	}
}	
/*!
 * DrvCanWriteIDMR
 * 写ID标识符屏蔽寄存器
 * @param[in]   bank：0-BANK1	!0-BANK2
				ptID：屏蔽值指针
 * @param[out]  None
 * @retval  	None
 */
void DrvCanWriteIDMR(unsigned char bank,unsigned char *ptID)
{
	if(bank){
		MSCAN->CANIDMR_BANK_2[0] = ptID[3];
		MSCAN->CANIDMR_BANK_2[1] = ptID[2];
		MSCAN->CANIDMR_BANK_2[2] = ptID[1];
		MSCAN->CANIDMR_BANK_2[3] = ptID[0];		
	}else{
		MSCAN->CANIDMR_BANK_1[0] = ptID[3];
		MSCAN->CANIDMR_BANK_1[1] = ptID[2];
		MSCAN->CANIDMR_BANK_1[2] = ptID[1];
		MSCAN->CANIDMR_BANK_1[3] = ptID[0];	
	}
}
/*!
 * DrvCanSetBaudRate
 * 设置波特率
 * @param[in]   br：波特率值
 * @param[out]  None
 * @retval  	None
 */
void DrvCanSetBaudRate(uint32_t br)
{
	uint32_t priDiv = br * MSCAN_TIME_QUANTA_NUM;
    if (0 == priDiv){
        priDiv = 1;
    }
	priDiv = (getBusClock() / priDiv) - 1;
    if (priDiv > 0x3F){
        priDiv = 0x3F;
    }	
    /* MsCAN timing setting formula:4 3
     * MSCAN_TIME_QUANTA_NUM = 1 + (TSEG1 + 1) + (TSEG2 + 1);
     */	
	DrvCanEnterInitMode();
    /* Cleaning previous Timing Setting. */
    MSCAN->CANBTR0 &= ~(MSCAN_CANBTR0_BRP_MASK | MSCAN_CANBTR0_SJW_MASK);
    MSCAN->CANBTR1 &= ~(MSCAN_CANBTR1_TSEG1_MASK | MSCAN_CANBTR1_TSEG2_MASK | MSCAN_CANBTR1_SAMP_MASK);	
	
    /* Updating Timing Setting according to configuration structure. */
    MSCAN->CANBTR0 |= (MSCAN_CANBTR0_BRP(priDiv) | MSCAN_CANBTR0_SJW(0));
    MSCAN->CANBTR1 |= (MSCAN_CANBTR1_TSEG1(4) | MSCAN_CANBTR1_TSEG2(3) |
                      MSCAN_CANBTR1_SAMP(0));	
	DrvCanExitInitMode();
}
/*!
 * DrvCanEnterInitMode
 * 进入初始化模式
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvCanEnterInitMode(void)
{
    /* Set initial request bit. */
    MSCAN->CANCTL0 |= MSCAN_CANCTL0_INITRQ_MASK;	
    /* Wait until the MsCAN Module enters initial mode. */
    while (!(MSCAN->CANCTL1 & MSCAN_CANCTL1_INITAK_MASK)){
    }	
}
/*!
 * DrvCanExitInitMode
 * 退出初始化模式
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DrvCanExitInitMode(void)
{
    /* Clear initial request bit. */
    MSCAN->CANCTL0 &= ~MSCAN_CANCTL0_INITRQ_MASK;
    /* Wait until the MsCAN Module exits initial mode. */
    while (MSCAN->CANCTL1 & MSCAN_CANCTL1_INITAK_MASK){
    }
}
/*!
 * DrvCanInit
 * CAN模块初始化
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvCanInit(unsigned long baudrate,s_USRDATA_T *usrdata)
{
	uint8_t ctl0Temp, ctl1Temp;
	uint32_t id;
	uint32_t mask;
	ptUsr = usrdata;
	/*gpio periphearl clock*/
	PeripheralClockEnable(PCC_PORTE_INDEX);	
	/*PORTE5 (physics pin 5)is configured as CAN0_TX*/
	DrvGpioSetPinMux(PORTE,PORTE_PIN5,PIN3_CAN0_TX);
	/*PORTE4 (physics pin 4)is configured as CAN0_RX*/
	DrvGpioSetPinMux(PORTE,PORTE_PIN4,PIN4_CAN0_RX); 
	/*Enable MSCAN clock*/
	PeripheralClockEnable(PCC_MSCAN0_INDEX);	
	/* Enable MsCAN Module for configuartion. */
	MSCAN->CANCTL1 |= MSCAN_CANCTL1_CANE_MASK;
	
	DrvCanEnterInitMode();

    ctl0Temp = MSCAN->CANCTL0;
    ctl1Temp = MSCAN->CANCTL1;	
	/*Disable Self Wake up mode*/
	ctl0Temp &= ~MSCAN_CANCTL0_WUPE_MASK;
	/* Disable Loop Back Mode. */
	ctl1Temp &= ~MSCAN_CANCTL1_LOOPB_MASK;
	/* Disable Listen Mode. */
	ctl1Temp &= ~MSCAN_CANCTL1_LISTEN_MASK;	
	/* Clock source selection. bus clock */
	ctl1Temp |= MSCAN_CANCTL1_CLKSRC_MASK;
    /* Save CTLx Configuation. */
    MSCAN->CANCTL0 = ctl0Temp;
    MSCAN->CANCTL1 = ctl1Temp;	
	
	/* Configure ID acceptance filter setting. */
	/*0-Two 32-bit acceptance filters
	  1-Four 16-bit accptance filters
	  2-Eight 8-bit accptance filters
	  3-Filter closed
	*/
	MSCAN->CANIDAC |= MSCAN_CANIDAC_IDAM(0);
	
	if(ptUsr->recvId1&EXT_CANFRAME_TYPE){
		id = MSCAN_RX_MB_EXT_MASK(ptUsr->recvId1&0x7FFFFFFF);
		mask = MSCAN_RX_MB_EXT_MASK(ptUsr->recvId1Mask)|0x180001;
	}else{
		id = MSCAN_RX_MB_STD_MASK(ptUsr->recvId1);
		mask = MSCAN_RX_MB_STD_MASK(ptUsr->recvId1Mask)|0x1F;
	}
	DrvCanWriteIDAR(0,(unsigned char*)&id);
	DrvCanWriteIDMR(0,(unsigned char*)&mask);
	
	if(ptUsr->recvId2&EXT_CANFRAME_TYPE){
		id = MSCAN_RX_MB_EXT_MASK(ptUsr->recvId2&0x7FFFFFFF);
		mask = MSCAN_RX_MB_EXT_MASK(ptUsr->recvId2Mask)|0x180001;
	}else{
		id = MSCAN_RX_MB_STD_MASK(ptUsr->recvId2);
		mask = MSCAN_RX_MB_STD_MASK(ptUsr->recvId2Mask)|0x1F;
	}	
	DrvCanWriteIDAR(1,(unsigned char*)&id);
	DrvCanWriteIDMR(1,(unsigned char*)&mask);
	/*set can baudrate 40k*/
	DrvCanSetBaudRate(baudrate);
	/* Enter normal mode. */
	DrvCanExitInitMode();
    /* Enable Timer. */
    MSCAN->CANCTL0 |= MSCAN_CANCTL0_TIME_MASK;
	MSCAN->CANRIER |= MSCAN_CANRIER_RXFIE_MASK|MSCAN_CANRIER_CSCIE_MASK|
						MSCAN_CANRIER_TSTATE(3)|MSCAN_CANRIER_RSTATE(3);
	NVIC_EnableIRQ(MSCAN_Rx_IRQn);
	NVIC_EnableIRQ(MSCAN_ORed_IRQn);
}
/*!
 * MSCAN_DriverIRQHandler
 * 接收中断处理
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void MSCAN_DriverIRQHandler(void)
{
	unsigned char lTemp;
	unsigned char lLen;
	
	lTemp = MSCAN->CANRFLG;
	if((lTemp & MSCAN_CANRFLG_RXF_MASK) == 0){
		if(lTemp&MSCAN_CANRFLG_CSCIF_MASK){//状态改变
			lTemp = ((lTemp&MSCAN_CANRFLG_RSTAT_MASK)>>4)&0x03;
			if((ptUsr->recvError)&&(lTemp != 0)){//接收故障处理
				ptUsr->recvError(lTemp);
			}
			lTemp = ((MSCAN->CANRFLG&MSCAN_CANRFLG_TSTAT_MASK)>>2)&0x03;
			if((ptUsr->sendError)&&(lTemp != 0)){//发送故障处理
				ptUsr->sendError(lTemp);
			}
			MSCAN->CANRFLG |= MSCAN_CANRFLG_CSCIF_MASK;
		}
		return;
	}
	lLen = MSCAN->RDLR & 0x0F;
	lTemp = MSCAN->REIDR1;
	ptUsr->ptFrame->Flag = lLen;
	if(lTemp&MSCAN_REIDR1_REIDE_MASK){//接收到扩展帧
		ptUsr->ptFrame->Flag |= EXT_CANFRAME_TYPE;
		ptUsr->ptFrame->Id = MSCAN->REIDR0;/*ID28-ID21*/
		ptUsr->ptFrame->Id <<= 21;
		ptUsr->ptFrame->Id |= (((lTemp>>2)&0x38)|(lTemp&0x07))<<15;/*ID20-ID15*/
		ptUsr->ptFrame->Id |= (MSCAN->REIDR2)<<7;
		ptUsr->ptFrame->Id |= (MSCAN->REIDR3)>>1;
	}else{
		ptUsr->ptFrame->Id = MSCAN->REIDR0;
		ptUsr->ptFrame->Id <<= 3;
		ptUsr->ptFrame->Id |= ((lTemp)>>5)&0x07;
	}
	for(lTemp = 0; lTemp < lLen; lTemp++){
		ptUsr->ptFrame->Data[lTemp] = MSCAN->REDSR[lTemp];
	}
	MSCAN->CANRFLG |= MSCAN_CANRFLG_RXF_MASK;
	ptUsr->recvStatus = 1;
	if(ptUsr->recvIRQ){
		ptUsr->recvIRQ();
	}
}
/*!
 * DrvCanSendFrame
 * 发送CAN帧数据
 * @param[in]   ptFrame：帧数据指针
 * @param[out]  None
 * @retval  	None
 */
void DrvCanSendFrame(const s_CANFRAME_T *ptFrame)
{
	unsigned char temp;
	unsigned char len;
	unsigned long id;
	
	len = ptFrame->Flag&0xFF;
	id = ptFrame->Id;
	temp = MSCAN->CANTFLG & MSCAN_CANTFLG_TXE_MASK;
	if((temp == 0)||(len > 8))//无有效发送缓冲区,数据长度异常
		return;
	MSCAN->CANTBSEL = MSCAN_CANTBSEL_TX(temp);//选择发送缓冲区	
	if(ptFrame->Flag&EXT_CANFRAME_TYPE){//发送扩展帧
		//填充扩展标识符
		MSCAN->TEIDR0 = (id>>21)&0xFF;/*ID28-ID21*/
		temp = (id>>15)&0x3F;/*ID20-ID15*/
		/*
			bit	7	6	  5	  4	   3   2   1    0
				|ID20-ID18| SRR | IDE |ID17-ID15|
		*/
		MSCAN->TEIDR1 = ((temp<<2)&0xE0)|MSCAN_TEIDR1_TSRR_MASK|MSCAN_TEIDR1_TEIDE_MASK|(temp&0x07);
		MSCAN->TEIDR2 = (id>>7)&0xFF;/*ID14-ID7*/
		MSCAN->TEIDR3 = (id<<1)&0xFE;/*ID6-ID0*/	
	}else{
		//填充标准标识符
		MSCAN->TSIDR0 = (id>>3)&0xFF;/*ID10-ID3*/
		MSCAN->TSIDR1 = (id<<5)&0xE0;/*ID2-ID0*/	
	}
	for(temp = 0; temp < len; temp++){//填充数据
		MSCAN->TEDSR[temp] = ptFrame->Data[temp];
	}
	MSCAN->TDLR = len;//填充长度
	MSCAN->CANTFLG = MSCAN_CANTFLG_TXE_MASK & (MSCAN->CANTBSEL&MSCAN_CANTBSEL_TX_MASK);	
}

