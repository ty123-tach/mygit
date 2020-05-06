/*****************************************************************************
	Copyright (C), 2016-2019, SU ZHOU HEYANG Co., Ltd.
	File name: System.c
	Author: Zhang zhiwei Version: V0 Date: 27. June. 2019
	Description: 
	Version: V0
	Function List: 

	History: 
	<author>  <time>  <version >  <desc>
******************************************************************************/
//* Private include files -----------------------------------------------------
//* @{引用头文件声明区域(仅限本文件内使用)
#include "System.h"
#include "MKE16Z4.h"
#include "DrvTimer.h"
#include "DrvUart.h"
//**
// * @}

//* Private Macros and Data type -----------------------------------------------
//* @{私有宏和数据定义区域(仅限本文件内使用的全局变量和宏)
/* Value of the external crystal or oscillator clock frequency in Hz */
#define CPU_XTAL_CLK_HZ                8000000u      
/* Default System clock value */
#define DEFAULT_SYSTEM_CLOCK           48000000u           

typedef void (*pFunc)(void);

typedef struct _scg_config_{
	unsigned char enable;
	unsigned char div1;
	unsigned char div2;
	unsigned char range;
}s_SCGCFG_T;
//**
// * @}

//* Public function declaration ------------------------------------------------
//* @{私有函数声明区域(仅限本文件内使用)

//**
// * @}
#ifdef VECTOR_MATO_RAM
extern uint32_t Image$$VECTOR_ROM_MAP$$Base[];
extern uint32_t Image$$VECTOR_RAM_MAP$$Base[];
extern uint32_t Image$$DATA_MAP$$Base[];
#define __VECTOR_TABLE Image$$VECTOR_ROM_MAP$$Base
#define __VECTOR_RAM Image$$VECTOR_RAM_MAP$$Base
#define __RAM_VECTOR_TABLE_SIZE (((uint32_t)Image$$DATA_MAP$$Base - (uint32_t)Image$$VECTOR_RAM_MAP$$Base))
void InterruptMap(void)
{
	uint32_t n;
	uint32_t len = ((uint32_t)__RAM_VECTOR_TABLE_SIZE) / sizeof(uint32_t);
	/* Copy the vector table from ROM to RAM */
	for (n = 0; n < len; n++)
	{
		__VECTOR_RAM[n] = __VECTOR_TABLE[n];
	}
	/* Point the VTOR to the position of vector table */
	SCB->VTOR = (uint32_t)__VECTOR_RAM;
}
#endif
/*!
 * SysSircClkInit
 * SIRC时钟初始化
 * @param[in]   config：时钟配置
 * @param[out]  None
 * @retval  	None
 */
void SysSircClkInit(s_SCGCFG_T *config)
{
	/* De-init the SIRC first. */
	if(SCG->SIRCCSR&SCG_SIRCCSR_SIRCSEL_MASK) /* If clock is used by system, return error. */
		return;
	if(SCG->SIRCCSR&SCG_SIRCCSR_LK_MASK)/* If configure register is locked, return error. */
		return;
	/* De-init the SIRC first. */
	SCG->SIRCCSR = 0U;
    /* Now start to set up SIRC clock. */
    /* Step 1. Setup dividers. */
    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV1(config->div1) | SCG_SIRCDIV_SIRCDIV2(config->div2);

    /* Step 2. Set SIRC configuration. */
    SCG->SIRCCFG = SCG_SIRCCFG_RANGE(config->range);

    /* Step 3. Enable clock. */
    SCG->SIRCCSR = SCG_SIRCCSR_SIRCEN_MASK;

    /* Step 4. Wait for SIRC clock to be valid. */
    while (!(SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK));
}
/*!
 * SysFircClkInit
 * FIRC时钟初始化
 * @param[in]   config：时钟配置
 * @param[out]  None
 * @retval  	None
 */
void SysFircClkInit(s_SCGCFG_T *config)
{
	uint32_t reg = SCG->FIRCCSR;
	/* If clock is used by system, return error. */
	if(reg&SCG_FIRCCSR_FIRCSEL_MASK)
		return;
    /* If configure register is locked, return error. */
    if (reg & SCG_FIRCCSR_LK_MASK)
        return;
	SCG->FIRCCSR = SCG_FIRCCSR_FIRCERR_MASK;
    /* Now start to set up FIRC clock. */
    /* Step 1. Setup dividers. */
    SCG->FIRCDIV = SCG_FIRCDIV_FIRCDIV1(config->div1) | SCG_FIRCDIV_FIRCDIV2(config->div2);
    /* Step 2. Set FIRC configuration. */
    SCG->FIRCCFG = SCG_FIRCCFG_RANGE(config->range);
    /* Step 3. Set trimming configuration. */
//    if (config->trimConfig)
//    {
//        SCG->FIRCTCFG =
//            SCG_FIRCTCFG_TRIMDIV(config->trimConfig->trimDiv) | SCG_FIRCTCFG_TRIMSRC(config->trimConfig->trimSrc);

//        /* TODO: Write FIRCSTAT cause bus error: TKT266932. */
//        if (kSCG_FircTrimNonUpdate == config->trimConfig->trimMode)
//        {
//            SCG->FIRCSTAT = SCG_FIRCSTAT_TRIMCOAR(config->trimConfig->trimCoar) |
//                            SCG_FIRCSTAT_TRIMFINE(config->trimConfig->trimFine);
//        }

//        /* trim mode. */
//        SCG->FIRCCSR = config->trimConfig->trimMode;

//        if (SCG->FIRCCSR & SCG_FIRCCSR_FIRCERR_MASK)
//        {
//            return kStatus_Fail;
//        }
//    }
    /* Step 4. Enable clock. */
    SCG->FIRCCSR |= SCG_FIRCCSR_FIRCEN_MASK;
    /* Step 5. Wait for FIRC clock to be valid. */
    while (!(SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK));	
}
/*!
 * PeripheralClockSrcSelect
 * 外设时钟源选择
 * @param[in]   perinum：外设编号
				clksrc：时钟源
 * @param[out]  None
 * @retval  	None
 */
void PeripheralClockSrcSelect(unsigned int perinum,unsigned char clksrc)
{
	unsigned int reg = PCC->CLKCFG[perinum];

	reg = (reg&(~PCC_CLKCFG_PCS_MASK))|PCC_CLKCFG_PCS(clksrc);
	/*如果外设时钟已使能，先关闭再使能*/
	PCC->CLKCFG[perinum] = reg&(~PCC_CLKCFG_CGC_MASK);
	PCC->CLKCFG[perinum] = reg|PCC_CLKCFG_CGC_MASK;
}
/*!
 * PeripheralClockEnable
 * 使能外设时钟
 * @param[in]   perinum：外设编号
 * @param[out]  None
 * @retval  	None
 */
void PeripheralClockEnable(unsigned int perinum)
{
	PCC->CLKCFG[perinum] |= PCC_CLKCFG_CGC_MASK;
}
/*!
 * SystemSetup
 * 系统设置
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void SystemSetup(void)
{
	s_SCGCFG_T cfg;
#ifdef VECTOR_MATO_RAM
	InterruptMap();	
#endif
	WDOG->CNT = WDOG_UPDATE_KEY;
	WDOG->TOVAL = 0xFFFF;
	WDOG->CS = (uint32_t) ((WDOG->CS) & ~WDOG_CS_EN_MASK) | WDOG_CS_UPDATE_MASK;
	cfg.div1 = 0;
	cfg.div2 = 2;
	cfg.range = 1;
	/* Init Sirc. */
	SysSircClkInit(&cfg);
	/* Change to use SIRC as system clock source to prepare to change FIRCCFG register. */
	SCG->RCCR = SCG_RCCR_SCS(CLK_SIRC_SRC)|SCG_RCCR_DIVCORE(0)|SCG_RCCR_DIVSLOW(3);
	/* Wait for clock source switch finished. */
	while ((SCG->CSR&SCG_CSR_SCS_MASK) != (CLK_SIRC_SRC<<SCG_CSR_SCS_SHIFT));
	/* Init Firc. */
	cfg.div2 = 1;
	cfg.range = 0;
	cfg.div1 = 1;
	SysFircClkInit(&cfg);
	 /* Change back to use FIRC as system clock source in order to configure SIRC if needed. */
	SCG->RCCR = SCG_RCCR_SCS(CLK_FIRC_SRC)|SCG_RCCR_DIVCORE(0)|SCG_RCCR_DIVSLOW(3);
	/* Wait for clock source switch finished. */
	while ((SCG->CSR&SCG_CSR_SCS_MASK) != (CLK_FIRC_SRC<<SCG_CSR_SCS_SHIFT));
	/*De-init the SIRC. */
	if(SCG->SIRCCSR&SCG_SIRCCSR_SIRCSEL_MASK) /* If clock is used by system, return error. */
		return;
	if(SCG->SIRCCSR&SCG_SIRCCSR_LK_MASK)/* If configure register is locked, return error. */
		return;
	SCG->SIRCCSR = 0U;		
}
/*!
 * JumpToApplication
 * jump from boot to app
 * @param[in]   None
 * @param[out]  None
 * @retval  	None
 */
void JumpToApplication(void)
{
	unsigned long JumpAdr;
	pFunc Jump;
	
	__disable_irq();
	DrvTimerDeinit();
	DrvUartDeinit();
    // relocate vector table
    SCB->VTOR = 0x00003000;	
	//set up stack pointer
	__set_MSP(*(__IO unsigned long*)0x00003000);
	__set_PSP(*(__IO unsigned long*)0x00003000);
	//Jump to PC
	JumpAdr = *(__IO unsigned long*)(0x00003000+4);
	Jump = (pFunc)JumpAdr;
	Jump();
}
