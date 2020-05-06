/*****************************************************************************
	Copyright (C), 2019- SU ZHOU HEYANG ZHIKONG Co., Ltd.
	File name: DrvFlash.c
	Author: ZhangZhiwei Version: V0 Date: 2019-07-22
	Description:
	Version: V0
	Function List:
 
	History:
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{����ͷ�ļ���������(���ޱ��ļ���ʹ��)
#include "DrvFlash.h"
#include "MKE16Z4.h"
#include "Configure.h"
//**
// * @}
 
//* Private Macros and Data type -----------------------------------------------
//* @{˽�к�����ݶ�������(���ޱ��ļ���ʹ�õ�ȫ�ֱ����ͺ�)
#define FAFT_ERASE_SECTOR 0x09U                    /*!< ERSSCR*/
#define FAFT_PROGRAM_LONGWORD 0x06U                /*!< PGM4*/
static unsigned char gFlashSecurityState = 0;	//Flash��ȫ״̬ 0-��ȫ	1-����ȫ 
//**
// * @}
 
//* Public function declaration ------------------------------------------------
//* @{˽�к�����������(���ޱ��ļ���ʹ��)
unsigned char FlashCommandSequence(void); 
//**
// * @}
/*!
 * DrvFlashWriteLongWord
 * erase flash sector
 * ����Flash����
 * @param[in]   sector��������
 * @param[out]  None
 * @retval  	 None
 */
void DrvFlashWriteLongWord(unsigned int adr,unsigned int data)
{
	volatile uint32_t *const kFCCOBx = (volatile uint32_t *)&(FTFA->FCCOB3);
	
	kFCCOBx[1] = data;//�������
	kFCCOBx[0] = (FAFT_PROGRAM_LONGWORD<<24)|(adr&0xFFFFFFU);//�������͵�ַ
	FlashCommandSequence();
}
/*!
 * DrvFlashEraseSector
 * erase flash sector
 * ����Flash����
 * @param[in]   sector��������
 * @param[out]  None
 * @retval  	 None
 */
void DrvFlashEraseSector(unsigned char sector)
{
	volatile uint32_t *const kFCCOBx = (volatile uint32_t *)&(FTFA->FCCOB3);
	uint32_t lStartAdr;
	
	lStartAdr = sector*CFG_FLASH_SECTORSIZE;
	if(sector >= CFG_FLASH_TOTAL_SECTORNUM)
		return;
	kFCCOBx[0] = (FAFT_ERASE_SECTOR<<24)|(lStartAdr&0xFFFFFFU);
	FlashCommandSequence();
}	
/*!
 * DrvFlashInit
 * init flash
 * ��ʼ��flash
 * @param[in]   None
 * @param[out]  None
 * @retval  	 None
 */
void DrvFlashInit(void)
{
	/* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register */
	FTFA->FSTAT = FTFA_FSTAT_RDCOLERR_MASK | FTFA_FSTAT_ACCERR_MASK | FTFA_FSTAT_FPVIOL_MASK;
	if((FTFA->FSEC&FTFA_FSEC_SEC_MASK) == 0x02){
		gFlashSecurityState = 1;
	}else{
		gFlashSecurityState = 0;
	}
}
/*!
 * FlashCommandSequence
 * execution command sequence
 * ִ��flash����
 * @param[in]   None
 * @param[out]  None
 * @retval  	0��ʧ��	1���ɹ�
 */
unsigned char FlashCommandSequence(void)
{
	uint8_t reg;
	if(gFlashSecurityState == 0)
		return 0;
	/* clear RDCOLERR & ACCERR & FPVIOL flag in flash status register */
	FTFA->FSTAT = FTFA_FSTAT_RDCOLERR_MASK | FTFA_FSTAT_ACCERR_MASK | FTFA_FSTAT_FPVIOL_MASK;
	/* clear CCIF bit */
	__disable_irq();
	FTFA->FSTAT = FTFA_FSTAT_CCIF_MASK;
	/* Check CCIF bit of the flash status register, wait till it is set.
	* IP team indicates that this loop will always complete. */
	while(!(FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK)){
	}
	__enable_irq();
	/* Check error bits */
	reg = FTFA->FSTAT;/* Get flash status register value */
	/* checking access error */
	if(reg & FTFA_FSTAT_ACCERR_MASK){
		return 0;
	}else if (reg & FTFA_FSTAT_FPVIOL_MASK){/* checking protection error */
		return 0;
	}else if (reg & FTFA_FSTAT_MGSTAT0_MASK){/* checking MGSTAT0 non-correctable error */
		return 0;
	}else{
		return 1;
	}
}
