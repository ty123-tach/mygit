/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: RslHbGpio.c
	Author: ZhangZhwei Version: V0 Date: 2019-09-02
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "RslHbGpio.h"
#include "MKE16Z4.h"
#include "DrvTimer.h"
#include "DrvGpio.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
/*Ӳ����Ӧ�������*/
#define HBBIT_SW1_1_VALUE			(GPIOD->PDIR&GPIOPORT_PIN4_MASK)
#define HBBIT_SW1_2_VALUE			(GPIOD->PDIR&GPIOPORT_PIN3_MASK)
#define HBBIT_SW1_3_VALUE			(GPIOD->PDIR&GPIOPORT_PIN2_MASK)
#define HBBIT_SW1_4_VALUE			(GPIOA->PDIR&GPIOPORT_PIN3_MASK)
#define HBBIT_SW1_5_VALUE			(GPIOA->PDIR&GPIOPORT_PIN2_MASK)
#define HBBIT_SW1_6_VALUE			(GPIOA->PDIR&GPIOPORT_PIN0_MASK)
#define HBBIT_BTN_UP_VALUE			(GPIOA->PDIR&GPIOPORT_PIN6_MASK)
#define HBBIT_BTN_DN_VALUE			(GPIOC->PDIR&GPIOPORT_PIN0_MASK)
#define HBBIT_BTN_LOCK_VALUE		(GPIOE->PDIR&GPIOPORT_PIN5_MASK)
/*GPIO����״̬��gHbGpioInputλ״̬ӳ��*/
#define HBGPIO_SW1_1_MASK		(0x01U)
#define HBGPIO_SW1_1_SHIFT		(0U)
#define HBGPIO_SW1_2_MASK		(0x02U)
#define HBGPIO_SW1_2_SHIFT		(1U)
#define HBGPIO_SW1_3_MASK		(0x04U)
#define HBGPIO_SW1_3_SHIFT		(2U)
#define HBGPIO_SW1_4_MASK		(0x08U)
#define HBGPIO_SW1_4_SHIFT		(3U)
#define HBGPIO_SW1_5_MASK		(0x10U)
#define HBGPIO_SW1_5_SHIFT		(4U)
#define HBGPIO_SW1_6_MASK		(0x20U)
#define HBGPIO_SW1_6_SHIFT		(5U)
#define HBGPIO_SW_MASK			(0x3FU)
#define HBGPIO_BTN_UP_MASK		(0x40U)
#define HBGPIO_BTN_UP_SHIFT		(6U)
#define HBGPIO_BTN_DN_MASK		(0x80U)
#define HBGPIO_BTN_DN_SHIFT		(7U)
#define HBGPIO_BTN_LOCK_MASK	(0x100U)
#define HBGPIO_BTN_LOCK_SHIFT	(8U) 

static unsigned char gHbGpioPulse[9];		/*�����������*/
static unsigned short gHbGpioInput = 0;		//HB GPIO����ֵ
#define MAX_HBGPIO_SPACE	10				/*�������,��λms*/
#define MAX_HBGPIO_PULSE	5
static unsigned int gHbGpioTimeStamp = 0;	/*����ʱ���*/ 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void HbGpioInputBitCheck(unsigned char pin,unsigned char sta); 
//**
// * @}
/*!
 * GetHbButtonValue
 * ��ȡHB��ťֵ
 * @param[in]   None
 * @param[out]  None
 * @retval  	���°�ťֵ
 */
unsigned char GetHbButtonValue(void)
{
	return gHbGpioInput&0xC0;
}
/*!
 * GetHbGpioKeyStatus
 * ��ȡHB��ť״̬
 * @param[in]   None
 * @param[out]  None
 * @retval  	1������	0-�ɿ�
 */
unsigned char GetHbGpioKeyStatus(unsigned short key)
{
	if(gHbGpioInput&key)
		return 1;
	return 0;
}
/*!
 * GetHbGpioDevAdr
 * ��ȡHB�豸��ַ
 * @param[in]   None
 * @param[out]  None
 * @retval  	�����豸��ַ:0-63
 */
unsigned char GetHbGpioDevAdr(void)
{
	return gHbGpioInput&HBGPIO_SW_MASK;
}
/*!
 * SetHbUpButtonStatus
 * �����ϰ�ť״̬
 * @param[in]   sta����������
 * @param[out]  None
 * @retval  	None
 */
void SetHbUpButtonStatus(unsigned char sta)
{
	if(sta){
		GPIOA->PSOR = 0x80;
	}else{
		GPIOA->PCOR = 0x80;
	}
}
/*!
 * SetHbDnButtonStatus
 * �����°�ť״̬
 * @param[in]   sta����������
 * @param[out]  None
 * @retval  	None
 */
void SetHbDnButtonStatus(unsigned char sta)
{
	if(sta){
		GPIOC->PSOR = 0x02;
	}else{
		GPIOC->PCOR = 0x02;
	}
} 
/*!
 * RslHbGpioInit
 * HBЭ��GPIO��ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void RslHbGpioInit(void)
{
	unsigned char lCnt;
	/*????SW1-6*/
    //оƬ����11��������ΪGPIO����
	DrvGpioSetPinMux(PORTD,PORTD_PIN4,PIN29_GPIO_PTD4);//PTD4 SW1_1
	//����Ϊ����ģʽ
	DrvGpioSetDirection(GPIOD,PORTD_PIN4,GPIO_DIR_INPUT); 
	
	DrvGpioSetPinMux(PORTD,PORTD_PIN3,PIN30_GPIO_PTD3);//PTD3 SW1_2
	DrvGpioSetDirection(GPIOD,PORTD_PIN3,GPIO_DIR_INPUT);	
	
	DrvGpioSetPinMux(PORTD,PORTD_PIN2,PIN31_GPIO_PTD2);//PTD2 SW1_3
	DrvGpioSetDirection(GPIOD,PORTD_PIN2,GPIO_DIR_INPUT);		
	
	DrvGpioSetPinMux(PORTA,PORTA_PIN3,PIN32_GPIO_PTA3);//PTA3 SW1_4
	DrvGpioSetDirection(GPIOA,PORTA_PIN3,GPIO_DIR_INPUT);	

	DrvGpioSetPinMux(PORTA,PORTA_PIN2,PIN33_GPIO_PTA2);//PTA2 SW1_5
	DrvGpioSetDirection(GPIOA,PORTA_PIN2,GPIO_DIR_INPUT);

	DrvGpioSetPinMux(PORTA,PORTA_PIN0,PIN35_GPIO_PTA0);//PTA0 SW1_6
	DrvGpioSetDirection(GPIOA,PORTA_PIN0,GPIO_DIR_INPUT);		
	
	DrvGpioSetPinMux(PORTA,PORTA_PIN6,PIN26_GPIO_PTA6);//PTA6 BTN_UP
	DrvGpioSetDirection(GPIOA,PORTA_PIN6,GPIO_DIR_INPUT);

	DrvGpioSetPinMux(PORTC,PORTD_PIN0,PIN20_GPIO_PTC0);//PTC0 BTN_DN
	DrvGpioSetDirection(GPIOC,PORTD_PIN0,GPIO_DIR_INPUT); 
	
	DrvGpioSetPinMux(PORTE,PORTE_PIN5,PIN3_GPIO_PTE5);//PTE5 BTN_LOCK
	DrvGpioSetDirection(GPIOE,PORTE_PIN5,GPIO_DIR_INPUT);	

    
	DrvGpioSetPinMux(PORTC,PORTC_PIN1,PIN19_GPIO_PTC1);//PTC1 BTN_DN_OUT
	//����Ϊ���ģʽ
	DrvGpioSetDirection(GPIOC,PORTC_PIN1,GPIO_DIR_OUTPUT); 	
	DrvGpioOutputLogicLevel(GPIOC,PORTC_PIN1,GPIO_LOW_LEVEL);//�����
	
	DrvGpioSetPinMux(PORTA,PORTA_PIN7,PIN25_GPIO_PTA7);//PTA7 BTN_UP_OUT
	DrvGpioSetDirection(GPIOA,PORTA_PIN7,GPIO_DIR_OUTPUT); 	
	DrvGpioOutputLogicLevel(GPIOA,PORTA_PIN7,GPIO_LOW_LEVEL);//�����
	
	for(lCnt = 0; lCnt < 100; lCnt++){
		gHbGpioTimeStamp = DrvGetTimerMsTick()-MAX_HBGPIO_SPACE;
		RslHbGpioInputHandle();
	}
	gHbGpioTimeStamp = DrvGetTimerMsTick();
}
/*!
 * RslHbGpioInputHandle
 * HB GPIO���봦��
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void RslHbGpioInputHandle(void)
{
	if((DrvGetTimerMsTick() - gHbGpioTimeStamp) < MAX_HBGPIO_SPACE)
		return;
	gHbGpioTimeStamp = DrvGetTimerMsTick();
	HbGpioInputBitCheck(HBGPIO_SW1_1_SHIFT,HBBIT_SW1_1_VALUE);
	HbGpioInputBitCheck(HBGPIO_SW1_2_SHIFT,HBBIT_SW1_2_VALUE);
	HbGpioInputBitCheck(HBGPIO_SW1_3_SHIFT,HBBIT_SW1_3_VALUE);
	HbGpioInputBitCheck(HBGPIO_SW1_4_SHIFT,HBBIT_SW1_4_VALUE);
	HbGpioInputBitCheck(HBGPIO_SW1_5_SHIFT,HBBIT_SW1_5_VALUE);
	HbGpioInputBitCheck(HBGPIO_SW1_6_SHIFT,HBBIT_SW1_6_VALUE);
	if(HBBIT_BTN_UP_VALUE){//��ť�ߵ�ƽ��Ч
		HbGpioInputBitCheck(HBGPIO_BTN_UP_SHIFT,0);
	}else{
		HbGpioInputBitCheck(HBGPIO_BTN_UP_SHIFT,1);
	}
	if(HBBIT_BTN_DN_VALUE){//��ť�ߵ�ƽ��Ч
		HbGpioInputBitCheck(HBGPIO_BTN_DN_SHIFT,0);
	}else{
		HbGpioInputBitCheck(HBGPIO_BTN_DN_SHIFT,1);
	}
	HbGpioInputBitCheck(HBGPIO_BTN_LOCK_SHIFT,HBBIT_BTN_LOCK_VALUE);	
}
/*!
 * HbGpioInputBitCheck
 * �������λ״̬(�͵�ƽ��Ч)
 * @param[in]   pin:��������	sta:״̬
 * @param[out]  None
 * @retval  	None
 */
void HbGpioInputBitCheck(unsigned char pin,unsigned char sta)
{
	if(sta){/*���ŵ�ƽΪ��*/
		if(gHbGpioPulse[pin] == MAX_HBGPIO_PULSE){/*ȥ��*/
			gHbGpioInput &= ~(1<<pin);
		}else{
			gHbGpioPulse[pin]++;		
		}
	}else{/*���ŵ�ƽΪ��,������*/
		if(gHbGpioPulse[pin] == 0){
			gHbGpioInput |= 1<<pin;
		}else if(gHbGpioPulse[pin] >= MAX_HBGPIO_PULSE){
			gHbGpioPulse[pin] = MAX_HBGPIO_PULSE-1;
		}else{
			gHbGpioPulse[pin]--;
		}
	}
}
