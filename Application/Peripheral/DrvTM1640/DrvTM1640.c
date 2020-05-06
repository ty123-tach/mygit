/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvTM1640.c
	Author: Li Hongpu Version: V0 Date: 2019-07-17
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "DrvTM1640.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
#define TM1640_SCK_H        GPIOA->PSOR = 1<<3U;
#define TM1640_SCK_L        GPIOA->PCOR = 1<<3U;

#define TM1640_DIN_H        GPIOA->PSOR = 1<<2U;
#define TM1640_DIN_L        GPIOA->PCOR = 1<<2U; 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)
void DrvTM1640SendByte(unsigned char lDat); 
void DrvTM1640WriteMem(void *dat,unsigned char dat_size);
void DrvTM1640Start(void);
void DrvTM1640End(void);
//**
// * @}
/*!
 * DrvTM1640Init
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvTM1640Delay1us(void)
{
    unsigned int i =4;
    while(i--);
} 
/*!
 * DrvTM1640Init
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvTM1640Init(void)
{
    //芯片物理33号引脚设置为GPIO功能
	DrvGpioSetPinMux(PORTA,PORTA_PIN3,PIN32_GPIO_PTA3);//PTA3   GPIO   SCK
    //设置为输出
	DrvGpioSetDirection(GPIOA,PORTA_PIN3,GPIO_DIR_OUTPUT);
	//输出高
	DrvGpioOutputLogicLevel(GPIOA,PORTA_PIN3,GPIO_HIGHT_LEVEL);   
 
    //芯片物理32号引脚设置为GPIO功能
    DrvGpioSetPinMux(PORTA,PORTA_PIN2,PIN33_GPIO_PTA2);//PTA2   GPIO   DIN 
    //设置为输出
	DrvGpioSetDirection(GPIOA,PORTA_PIN2,GPIO_DIR_OUTPUT);
	//输出高
	DrvGpioOutputLogicLevel(GPIOA,PORTA_PIN2,GPIO_HIGHT_LEVEL);   
	DrvTm1640Clear();
}
/*!
 * DrvTm1640Clear
   清空TM1640显示内容
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvTm1640Clear(void)
{
	unsigned char lCnt = 0;
	DrvTM1640Start();
    DrvTM1640SendByte(0x40);//CMD1
    DrvTM1640End();
	
	DrvTM1640Start();
    DrvTM1640SendByte(0xc0);//CMD2
	
	for(lCnt = 0; lCnt <16; lCnt++){
		DrvTM1640SendByte(0x00);
	}
	DrvTM1640End();	
    DrvTM1640Start();
    DrvTM1640SendByte(0x8F);//CMD3
    DrvTM1640End();		
}
/*!
 * DrvTM1640Start
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvTM1640Start(void)
{
    TM1640_DIN_H
    TM1640_SCK_H
    DrvTM1640Delay1us();
    TM1640_DIN_L
    DrvTM1640Delay1us();
	DrvTM1640Delay1us();
    TM1640_SCK_L
}
/*!
 * DrvTM1640End
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvTM1640End(void)
{
    TM1640_SCK_L
    TM1640_DIN_L
    DrvTM1640Delay1us();
    TM1640_SCK_H
	DrvTM1640Delay1us();
    DrvTM1640Delay1us();
    TM1640_DIN_H
}
/*!
 * DrvTM1640Start
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvTM1640SendByte(unsigned char lDat)
{
    unsigned char i =8;
    TM1640_SCK_L
    while(i--)
    {
        DrvTM1640Delay1us();
        if(lDat & 0x01)
            TM1640_DIN_H
        else
            TM1640_DIN_L
        lDat >>= 1;
        DrvTM1640Delay1us();
        TM1640_SCK_H
        DrvTM1640Delay1us();
        DrvTM1640Delay1us();
        TM1640_SCK_L
    }
}
/*!
 * DrvTM1640WriteMem
 * @param[in]   dat：待写入的数
				dat_size：数据的宽度
 * @param[out]  None
 * @retval  	None
 */
void DrvTM1640WriteMem(void *dat,unsigned char dat_size)
{
	unsigned char i = 8;
	unsigned char len;
	unsigned char lDat;
	unsigned char *ptDat = (unsigned char *)dat;
	
	for(len = 0; len < 16; len++){
		lDat = *ptDat;
		TM1640_SCK_L
		for(i = 0; i < 8; i++){
			DrvTM1640Delay1us();
			if(lDat & 0x01)
				TM1640_DIN_H
			else
				TM1640_DIN_L
			lDat >>= 1;
			DrvTM1640Delay1us();
			TM1640_SCK_H
			DrvTM1640Delay1us();
			DrvTM1640Delay1us();
			TM1640_SCK_L			
		}
		ptDat += dat_size;
	}
}
/*!
 * DrvTM1640SendData
 * @param[in]   dat：待发送的数据
				dat_size：待发送的数据类型大小
				dat_len：待发送
 * @param[out]  None
 * @retval  	None
 */
void DrvTM1640SendData(void *dat,unsigned char dat_size,unsigned char dat_len)
{
	DrvTM1640Start();
    DrvTM1640SendByte(0x40);//CMD1
    DrvTM1640End();
	
	DrvTM1640Start();
    DrvTM1640SendByte(0xc0);//CMD2   
	
	DrvTM1640WriteMem(dat,dat_size);

	DrvTM1640End();	
    DrvTM1640Start();
    DrvTM1640SendByte(0x8F);//CMD3
    DrvTM1640End();	
}
