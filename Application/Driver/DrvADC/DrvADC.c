/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvADC.c
	Author: ZhangZhiwei Version: V0 Date: 2019-08-09
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "DrvADC.h"
#include "MKE16Z4.h"
#include "DrvGpio.h"
#include "System.h"
#include "DrvTimer.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
pFuncGetAdcValue ptExtGetAdcValue[14];	//�ⲿ�����б�
static unsigned char gCurrentExtFunc = 0;
static unsigned char gIsUsing = 0;
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
void DefaultGetValue(unsigned short value);
//**
// * @}
/*!
 * DrvAdcWorkStatus
 * ��ȡADC����״̬
 * @param[in]   None
 * @param[out]  None
 * @retval  	1������ת��	0��ת�����
 */
unsigned char DrvAdcWorkStatus(void)
{
	return gIsUsing;
}
/*!
 * DrvAdcGetValueFuncRegister
 * ��ȡADCֵ�ⲿ����ע��
 * @param[in]   ch��ͨ��
				extfunc���ⲿ����
 * @param[out]  None
 * @retval  	1��ע��ʧ��	0��ע��ɹ�
 */
unsigned char DrvAdcGetValueFuncRegister(unsigned char ch,pFuncGetAdcValue extfunc)
{
	if(ch > 13)
		return 1;
	if(ptExtGetAdcValue[ch] != 0)//ͨ���ѱ�ע��
		return 1;
	ptExtGetAdcValue[ch] = extfunc;
	return 0;
}
/*!
 * DoAutoCalibration
 * �Զ�У׼
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DoAutoCalibration(void)
{
	unsigned int lSaveCfg1;
	unsigned int lSaveSc3;
	
	lSaveCfg1 = ADC0->CFG1;
	lSaveSc3 = ADC0->SC3;
	/* Set ADCK (ADC clock) to half the maximum specified frequency for calibration. */
	ADC0->CFG1 |= ADC_CFG1_ADIV(1);
	/* Trigger calibration and wait until it complete. */
	ADC0->SC3 |= ADC_SC3_CAL_MASK;
	while(ADC_SC1_COCO_MASK != (ADC0->SC1[0]&ADC_SC1_COCO_MASK)){
	
	}
	ADC0->R[0];
	ADC0->SC3 = lSaveSc3;
	ADC0->CFG1 = lSaveCfg1;
}	
/*!
 * DrvAdcInit
 * ADC��ʼ��
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvAdcInit(void)
{
	unsigned int lReg;

	gCurrentExtFunc = 0;
	gIsUsing = 0;
	ptExtGetAdcValue[gCurrentExtFunc] = DefaultGetValue;
	for(lReg = 1; lReg < 14; lReg++){
		ptExtGetAdcValue[lReg] = 0;
	}
//    //����оƬ14��������Ϊģ��ADC����
//	DrvGpioSetPinMux(PORTC,PORTC_PIN3,PIN14_ADC0_SE11_ACMP0_IN4);//PTC3  			
//    //����оƬ15��������Ϊģ��ADC����
//	DrvGpioSetPinMux(PORTC,PORTC_PIN2,PIN15_ADC0_SE10_ACMP0_IN5);//PTC2  		
	/*select clock source*/
	PeripheralClockSrcSelect(PCC_ADC0_INDEX,CLK_FIRC_SRC);
	/*CFG1*/
	lReg = ADC0->CFG1&(~(ADC_CFG1_ADICLK_MASK | ADC_CFG1_ADIV_MASK | ADC_CFG1_MODE_MASK));
	/*Input Clock Select: ALTCLK1
	  Conversion mode selection��12-bit conversion.
	  Clock Divide Select��1
	*/
	lReg |= ADC_CFG1_ADICLK(0)|ADC_CFG1_MODE(1)|ADC_CFG1_ADIV(0);
	ADC0->CFG1 = lReg;
	
    /* ADCx_CFG2. Set Sample time*/
    lReg = (ADC0->CFG2 & (~ADC_CFG2_SMPLTS_MASK));	
	lReg |= ADC_CFG2_SMPLTS(12);//12 ADICLK clock cycles 1/48Mhz *12 = 250ns
	ADC0->CFG2 = lReg;
	
	/* ADCx_SC2. voltage reference selection*/
	lReg = ADC0->SC2 & (~(ADC_SC2_REFSEL_MASK|ADC_SC2_ADTRG_MASK));
	/*Default voltage reference pin pair, that is, external pins VREFH and VREFL
		Software trigger selected.
	*/
	lReg |= ADC_SC2_REFSEL(0)|ADC_SC2_ADTRG(0);
	ADC0->SC2 = lReg;
	
	 /* ADCx_SC3. */
	 /*Disable continuous conversion*/
	lReg = ADC0->SC3 & (~(ADC_SC3_ADCO_MASK|ADC_SC3_AVGE_MASK|ADC_SC3_AVGS_MASK));
	/*Hardware Average Enable and 32 samples averaged.*/
	lReg |= ADC_SC3_AVGE(1)|ADC_SC3_AVGS(3);
	ADC0->SC3 = lReg;
	DoAutoCalibration();
	NVIC_EnableIRQ(ADC0_IRQn);
    NVIC_SetPriority(ADC0_IRQn,3);
}
/*!
 * DrvAdcTriggerConversion
 * ADC����ת��
 * @param[in]   channel��ͨ�� ��Χ0-11
 * @param[out]  None
 * @retval  	None
 */
void DrvAdcTriggerConversion(unsigned char channel)
{
	unsigned int lReg;
	unsigned char lRealCh;
	
	if((channel >= 14)||(channel == 0)||(gIsUsing == 1))
		return;
	lRealCh = channel -1;
	gCurrentExtFunc = channel;
	lReg = ADC0->SC1[0]&(~(ADC_SC1_ADCH_MASK | ADC_SC1_AIEN_MASK));
	lReg |= ADC_SC1_ADCH(lRealCh)|ADC_SC1_AIEN_MASK;
	ADC0->SC1[0] = lReg;
	gIsUsing = 1;
}
/*!
 * DefaultGetValue
 * Ĭ�ϻ�ȡADCֵ
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void DefaultGetValue(unsigned short value)
{

}
/*!
 * ADC0_IRQHandler
 * ADC�жϴ���
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void ADC0_IRQHandler(void)
{
	ptExtGetAdcValue[gCurrentExtFunc](ADC0->R[0]);
	gIsUsing = 0;
}
