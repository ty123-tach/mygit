/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: SpiLcd.c
	Author: ZhangZhwei Version: V0 Date: 2019-09-02
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "SpiLcd.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
#include "System.h"
#include "DrvTimer.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define MAX_LCDCOM_TOGGLE	8	/*62.5Hz*/
#define LCD_COM_OUTL	(GPIOC->PCOR = 0x02)
#define LCD_COM_OUTH	(GPIOC->PSOR = 0x02)

#define SPI_595OE_EN	(GPIOE->PCOR = 0x04)
#define SPI_595OE_DIS	(GPIOE->PSOR = 0x04)

#define SPI_595CS_EN	(GPIOA->PCOR = 0x01)
#define SPI_595CS_DIS	(GPIOA->PSOR = 0x01)

#define SPI_595CLK_H	(GPIOE->PSOR = 0x01)
#define SPI_595CLK_L	(GPIOE->PCOR = 0x01)

#define SPI_595DAT_H	(GPIOE->PSOR = 0x02)
#define SPI_595DAT_L	(GPIOE->PCOR = 0x02)

static unsigned char gLcdComToggle;
static unsigned char gLcdRamBuf[37];	//LCD RAM SEG
static unsigned int gLcdComTimeStamp = 0; 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void SpiLcdSendData(unsigned char toggle); 
//**
// * @}
/*!
 * SetSpiLcdRamSeg
 * ����SPI LCD RAM�е�SEG״̬
 * @param[in]   pos��SEG��RAM�е�λ��
				status��0Ϩ��	��0����
 * @param[out]  None
 * @retval  	None
 */
void SetSpiLcdRamSeg(unsigned char pos,unsigned char status)
{
	if(pos >= 37)
		return;
	gLcdRamBuf[pos] = status;
}	
/*!
 * SpiLcdInit
 * SPI LCD��ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void SpiLcdInit(void)
{
	DrvGpioSetPinMux(PORTC,PORTC_PIN1,PIN19_GPIO_PTC1);//PTC1 DIS_COM_MCU
	//����Ϊ���ģʽ
	DrvGpioSetDirection(GPIOC,PORTC_PIN1,GPIO_DIR_OUTPUT); 	
	DrvGpioOutputLogicLevel(GPIOC,PORTC_PIN1,GPIO_LOW_LEVEL);//�����
	
	DrvGpioSetPinMux(PORTE,PORTE_PIN2,PIN38_GPIO_PTE2);//PTE2 SPI OE
	//����Ϊ���ģʽ
	DrvGpioSetDirection(GPIOE,PORTE_PIN2,GPIO_DIR_OUTPUT); 	
	DrvGpioOutputLogicLevel(GPIOE,PORTE_PIN2,GPIO_HIGHT_LEVEL);//����ߣ�����ʹ��595	
	
	
	DrvGpioSetPinMux(PORTA,PORTA_PIN0,PIN35_GPIO_PTA0);//PTA0 SPI CS
	//����Ϊ���ģʽ
	DrvGpioSetDirection(GPIOA,PORTA_PIN0,GPIO_DIR_OUTPUT); 	
	DrvGpioOutputLogicLevel(GPIOA,PORTA_PIN0,GPIO_LOW_LEVEL);//�����,Ƭѡ595
	
	DrvGpioSetPinMux(PORTE,PORTE_PIN0,PIN40_GPIO_PTE0);//PTE0 SPI CLK
	//����Ϊ���ģʽ
	DrvGpioSetDirection(GPIOE,PORTE_PIN0,GPIO_DIR_OUTPUT); 	
	DrvGpioOutputLogicLevel(GPIOE,PORTE_PIN0,GPIO_LOW_LEVEL);//�����	
	
	DrvGpioSetPinMux(PORTE,PORTE_PIN1,PIN39_GPIO_PTE1);//PTE1 SPI DAT
	//����Ϊ���ģʽ
	DrvGpioSetDirection(GPIOE,PORTE_PIN1,GPIO_DIR_OUTPUT); 	
	DrvGpioOutputLogicLevel(GPIOE,PORTE_PIN1,GPIO_LOW_LEVEL);//�����	
	
	gLcdComTimeStamp =  DrvGetTimerMsTick();
	gLcdComToggle = 0;
	LCD_COM_OUTL;
	SpiLcdSendData(gLcdComToggle);
	SPI_595OE_EN;
}
/*!
 * SpiLcdSleep
 * ����
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SpiLcdSleep(void)
{
	unsigned char lCnt;
	
	for(lCnt = 0; lCnt< 37; lCnt++){
		gLcdRamBuf[lCnt] = 0;
	}
	gLcdComToggle = 0;
	LCD_COM_OUTL;
	SpiLcdSendData(gLcdComToggle);	
}
/*!
 * SpiLcdSendData
 * SPI��������
 * @param[in]   toggle����ת״̬
 * @param[out]  None
 * @retval  	None
 */
void SpiLcdSendData(unsigned char toggle)
{
	unsigned char lCnt;

	for(lCnt = 0; lCnt < 37; lCnt++){
		if(toggle){
			if(gLcdRamBuf[lCnt]){
				SPI_595DAT_L;
			}else{
				SPI_595DAT_H;
			}
		}else{
			if(gLcdRamBuf[lCnt]){
				SPI_595DAT_H;
			}else{
				SPI_595DAT_L;
			}		
		}
		SPI_595CLK_L;
		SPI_595CLK_H;		
	}
	SPI_595CS_EN;
	SPI_595CS_DIS;	
}
/*!
 * SpiLcdHandle
 * LCD��ʾ����
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void SpiLcdHandle(void)
{
	if((DrvGetTimerMsTick() - gLcdComTimeStamp) < MAX_LCDCOM_TOGGLE)
		return;
	gLcdComTimeStamp = DrvGetTimerMsTick();	
	gLcdComToggle ^= 1;	
	if(gLcdComToggle){
		LCD_COM_OUTH;
	}else{
		LCD_COM_OUTL;
	}
	SpiLcdSendData(gLcdComToggle);	
}
