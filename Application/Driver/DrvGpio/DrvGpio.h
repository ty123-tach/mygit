/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: DrvGpio.h
	Author: Zhang zhiwei Version: V0 Date: 04. July. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#ifndef _DRVGPIO_H_
#define _DRVGPIO_H_
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�
/*1�����Ź���*/
#define PIN1_TSIO_CH5		0U
#define PIN1_GPIO_PTD1		1U
#define PIN1_FTM0_CH3		2U
#define PIN1_TRGMUX_OUT2	7U
/*2�����Ź���*/
#define PIN2_TSIO_CH4		0U
#define PIN2_GPIO_PTD0		1U
#define PIN2_FTM0_CH2		2U
#define PIN2_TRGMUX_OUT1	7U
/*3�����Ź���*/
#define PIN3_TSIO_CH0		0U
#define PIN3_GPIO_PTE5		1U
#define PIN3_TCLK2  		2U
#define PIN3_CAN0_TX    	5U
#define PIN3_EWM_OUT_b      7U
/*4�����Ź���*/
#define PIN4_TSIO_CH1		0U
#define PIN4_GPIO_PTE4		1U
#define PIN4_BUSOUT  		2U
#define PIN4_CAN0_RX    	5U
#define PIN4_EWM_IN        	7U
/*9�����Ź���*/
#define PIN9_EXTAL  		0U
#define PIN9_GPIO_PTB7   	1U
#define PIN9_LPI2C0_SCL     2U
/*10�����Ź���*/
#define PIN10_XTAL  		0U
#define PIN10_GPIO_PTB6   	1U
#define PIN10_LPI2C0_SDA    2U
/*11�����Ź���*/
#define PIN11_TSI0_CH24  	0U
#define PIN11_GPIO_PTE3   	1U
#define PIN11_FTM0_FLT0     2U
#define PIN11_LPUART2_RTS   3U
/*12�����Ź���*/
#define PIN12_TSIO_CH9		0U
#define PIN12_GPIO_PTB5		1U
#define PIN12_FTM0_CH5		2U
#define PIN12_LPSPI0_PCS1	3U
#define PIN12_TRGMUX_IN0	6U
/*13�����Ź���*/
#define PIN13_TSIO_CH8		0U
#define PIN13_GPIO_PTB4		1U
#define PIN13_FTM0_CH4		2U
#define PIN13_LPSPI0_SOUT	3U
#define PIN13_TRGMUX_IN1	6U
/*14�����Ź���*/
#define PIN14_ADC0_SE11_ACMP0_IN4		0U
#define PIN14_GPIO_PTC3		1U
#define PIN14_FTM0_CH3		2U
/*15�����Ź���*/
#define PIN15_ADC0_SE10_ACMP0_IN5		0U
#define PIN15_GPIO_PTC2		1U
#define PIN15_FTM0_CH2		2U
/*16�����Ź���*/
#define PIN16_TSI0_CH10 	0U
#define PIN16_GPIO_PTD7		1U
#define PIN16_LPUART2_TX	2U
/*17�����Ź���*/
#define PIN17_TSI0_CH7   	0U
#define PIN17_GPIO_PTD6		1U
#define PIN17_LPUART2_RX	2U
/*18�����Ź���*/
#define PIN18_TSI0_CH6   	0U
#define PIN18_GPIO_PTD5		1U
#define PIN18_LPTMR0_ALT2	3U
#define PIN18_PWT_IN2	    5U
#define PIN18_LPUART2_CTS	7U
/*19�����Ź���*/
#define PIN19_ADC0_SE9_TSI0_CH23   	0U
#define PIN19_GPIO_PTC1		1U
#define PIN19_FTM0_CH1	    2U
/*20�����Ź���*/
#define PIN20_ADC0_SE8_TSI0_CH22   	0U
#define PIN20_GPIO_PTC0		1U
#define PIN20_FTM0_CH0	    2U
/*21�����Ź���*/
#define PIN21_ADC0_SE7_TSI0_CH21   	0U
#define PIN21_GPIO_PTB3		1U
#define PIN21_FTM1_CH1	    2U
#define PIN21_LPSPI0_SIN	3U
#define PIN21_FTM1_QD_PHA	4U
#define PIN21_TRGMUX_IN2	6U
/*22�����Ź���*/
#define PIN22_ADC0_SE6_TSI0_CH20   	0U
#define PIN22_GPIO_PTB2		1U
#define PIN22_FTM1_CH0	    2U
#define PIN22_LPSPI0_SCK	3U
#define PIN22_FTM1_QD_PHB	4U
#define PIN22_TRGMUX_IN3	6U
/*23�����Ź���*/
#define PIN23_ADC0_SE5  	0U
#define PIN23_GPIO_PTB1		1U
#define PIN23_LPUART0_TX	2U
#define PIN23_LPSPI0_SOUT	3U
#define PIN23_TCLK0     	4U
/*24�����Ź���*/
#define PIN24_ADC0_SE4  	0U
#define PIN24_GPIO_PTB0		1U
#define PIN24_LPUART0_RX	2U
#define PIN24_LPSPI0_PCSO	3U
#define PIN24_LPTMR0_ALT3   4U
#define PIN24_PWT_IN3       5U
/*25�����Ź���*/
#define PIN25_ADC0_SE3  	0U
#define PIN25_GPIO_PTA7		1U
#define PIN25_FTM0_FLT2 	2U
#define PIN25_LPSPI0_PCS3	3U
#define PIN25_RTC_CLKIN     4U
#define PIN25_LPUART1_RTS   6U
/*26�����Ź���*/
#define PIN26_ADC0_SE2  	0U
#define PIN26_GPIO_PTA6		1U
#define PIN26_FTM0_FLT1 	2U
#define PIN26_LPUART1_CTS   6U
/*29�����Ź���*/
#define PIN29_GPIO_PTD4		1U
#define PIN29_FTM0_FLT3 	2U
/*30�����Ź���*/
#define PIN30_GPIO_PTD3		1U
#define PIN30_NMI_b 	    7U
/*31�����Ź���*/
#define PIN31_GPIO_PTD2		1U
/*32�����Ź���*/
#define PIN32_GPIO_PTA3		1U
#define PIN32_LPI2C0_SCL	3U
#define PIN32_EWM_IN		4U
#define PIN32_LPUART0_TX	6U
/*33�����Ź���*/
#define PIN33_GPIO_PTA2		1U
#define PIN33_LPI2C0_SDA	3U
#define PIN33_EWM_OUT_b		4U
#define PIN33_LPUART0_RX	6U
/*34�����Ź���*/
#define PIN34_ADC0_SE1_ACMP0_IN1_TSI0_CH18 	0U
#define PIN34_GPIO_PTA1		1U
#define PIN34_FTM1_CH1  	2U
#define PIN34_LPI2C0_SDAS	3U
#define PIN34_FTM1_QD_PHA	5U
#define PIN34_LPUART0_RTS	6U
#define PIN34_TRGMUX_OUT0	7U
/*35�����Ź���*/
#define PIN35_ADC0_SE0_ACMP0_IN0_TSI0_CH17 	0U
#define PIN35_GPIO_PTA0		1U
#define PIN35_LPI2C0_SCLS	3U
#define PIN35_LPUART0_CTS	6U
#define PIN35_TRGMUX_OUT3	7U
/*36�����Ź���*/
#define PIN36_TSIO_CH16		0U
#define PIN36_GPIO_PTC7		1U
#define PIN36_LPUART1_TX	2U
#define PIN36_CAN0_TX		5U
/*37�����Ź���*/
#define PIN37_TSIO_CH15		0U
#define PIN37_GPIO_PTC6		1U
#define PIN37_LPUART1_RX	2U
#define PIN37_CAN0_RX		5U
/*38�����Ź���*/
#define PIN38_TSIO_CH19		0U
#define PIN38_GPIO_PTE2		1U
#define PIN38_LPSPI0_SOUT	2U
#define PIN38_LPTMR0_ALT3	3U
#define PIN38_PWT_IN3   	5U
#define PIN38_LPUART1_CTS	6U
/*39�����Ź���*/
#define PIN39_TSIO_CH14		0U
#define PIN39_GPIO_PTE1		1U
#define PIN39_LPSPI0_SIN	2U
#define PIN39_LPI2C0_HREQ	3U
/*40�����Ź���*/
#define PIN40_TSIO_CH13		0U
#define PIN40_GPIO_PTE0		1U
#define PIN40_LPSPI0_SCK	2U
#define PIN40_TCLK1     	3U
/*41�����Ź���*/
#define PIN41_TSIO_CH12		0U
#define PIN41_GPIO_PTC5		1U
#define PIN41_RTC_CLKOUT   	3U
/*42�����Ź���*/
#define PIN42_ACMP0_IN2		0U
#define PIN42_GPIO_PTC4		1U
#define PIN42_FTM1_CH0   	2U
#define PIN42_RTC_CLKOUT	3U
#define PIN42_EWM_IN		5U
#define PIN42_FTM1_QD_PHB  	6U
#define PIN42_SWD_CLK     	7U
/*43�����Ź���*/
#define PIN43_GPIO_PTA5		1U
#define PIN43_TCLK1 		3U
#define PIN43_RESET_b   	7U
/*44�����Ź���*/
#define PIN44_GPIO_PTA4		1U
#define PIN44_ACMP0_OUT		4U
#define PIN44_EWM_OUT_b   	5U
#define PIN44_SWD_DIO   	7U

#define GPIOPORT_PIN0_MASK	(0x01U)
#define GPIOPORT_PIN1_MASK	(0x02U)
#define GPIOPORT_PIN2_MASK	(0x04U)
#define GPIOPORT_PIN3_MASK	(0x08U)
#define GPIOPORT_PIN4_MASK	(0x10U)
#define GPIOPORT_PIN5_MASK	(0x20U)
#define GPIOPORT_PIN6_MASK	(0x40U)
#define GPIOPORT_PIN7_MASK	(0x80U)
#define GPIOPORT_PIN8_MASK	(0x100U)
#define GPIOPORT_PIN9_MASK	(0x200U)
#define GPIOPORT_PIN10_MASK	(0x400U)
#define GPIOPORT_PIN11_MASK	(0x800U)
#define GPIOPORT_PIN12_MASK	(0x1000U)
#define GPIOPORT_PIN13_MASK	(0x2000U)
#define GPIOPORT_PIN14_MASK	(0x4000U)
#define GPIOPORT_PIN15_MASK	(0x8000U)
/*GPIOA�����ű��*/
#define PORTA_PIN0	0
#define PORTA_PIN1	1
#define PORTA_PIN2	2
#define PORTA_PIN3	3
#define PORTA_PIN4	4
#define PORTA_PIN5	5
#define PORTA_PIN6	6
#define PORTA_PIN7	7
/*GPIOB�����ű��*/
#define PORTB_PIN0	0
#define PORTB_PIN1	1
#define PORTB_PIN2	2
#define PORTB_PIN3	3
#define PORTB_PIN4	4
#define PORTB_PIN5	5
#define PORTB_PIN6	6
#define PORTB_PIN7	7
/*GPIOC�����ű��*/
#define PORTC_PIN0	0
#define PORTC_PIN1	1
#define PORTC_PIN2	2
#define PORTC_PIN3	3
#define PORTC_PIN4	4
#define PORTC_PIN5	5
#define PORTC_PIN6	6
#define PORTC_PIN7	7
/*GPIOD�����ű��*/
#define PORTD_PIN0	0
#define PORTD_PIN1	1
#define PORTD_PIN2	2
#define PORTD_PIN3	3
#define PORTD_PIN4	4
#define PORTD_PIN5	5
#define PORTD_PIN6	6
#define PORTD_PIN7	7
/*GPIOE�����ű��*/
#define PORTE_PIN0	0
#define PORTE_PIN1	1
#define PORTE_PIN2	2
#define PORTE_PIN3	3
#define PORTE_PIN4	4
#define PORTE_PIN5	5
#define PORTE_PIN7	7
#define PORTE_PIN12	12

/*GPIO Direction*/
#define GPIO_DIR_INPUT 0
#define GPIO_DIR_OUTPUT 1

/*GPIO Logic Level*/
#define GPIO_LOW_LEVEL	0
#define GPIO_HIGHT_LEVEL 1

/*Interrupt configuration*/
#define TYPE_LOWLEVEL_INTERRUPT 	8	//�͵�ƽ�ж�
#define TYPE_RISING_INTERRUPT		9	//�������ж�
#define TYPE_FALLING_INTERRUPT		10	//�½����ж�
#define TYPE_EITHER_INTERRUPT		11	//˫�����ж�
#define TYPE_HIGHTLEVEL_INTERRUPT	12	//�ߵ�ƽ�ж�
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
/*
	port��ʹ�õ�GPIO��
	pin��GPIO���Ӧ�����ű��
	phy_pin_func��оƬʵ��ʹ�õ��������ű�Ź���
*/
void DrvGpioSetPinMux(void *port,unsigned char pin,unsigned char phy_pin_func);
/*
	gpio��ʹ�õ�GPIO��
	pin��GPIO���Ӧ�����ű��
	level���߼���ƽ 0-�͵�ƽ	��0-�ߵ�ƽ
*/
void DrvGpioOutputLogicLevel(void *gpio,unsigned char pin,unsigned char level);
/*
	gpio��ʹ�õ�GPIO��
	pin��GPIO���Ӧ�����ű��
	dir��0-GPIOΪ���뷽��	��0-GPIOΪ�������
*/
void DrvGpioSetDirection(void *gpio,unsigned char pin,unsigned char dir);
/*
	gpio��ʹ�õ�GPIO��
	pin��GPIO���Ӧ�����ű��
	dir��0-GPIOΪ���뷽��	��0-GPIOΪ�������
*/
void DrvGpioSetInterrupt(void *port,unsigned char pin,unsigned char type);
//**
// * @}
#endif
