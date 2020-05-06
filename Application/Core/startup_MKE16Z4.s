; * -------------------------------------------------------------------------
; *  @file:    startup_MKE16Z4.s
; *  @purpose: CMSIS Cortex-M0P Core Device Startup File
; *            MKE16Z4
; *  @version: 2.0
; *  @date:    2018-9-17
; *  @build:   b180925
; * -------------------------------------------------------------------------
; *
; * Copyright 1997-2016 Freescale Semiconductor, Inc.
; * Copyright 2016-2018 NXP
; * All rights reserved.
; *
; * SPDX-License-Identifier: BSD-3-Clause
; *
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; *****************************************************************************/


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                IMPORT  |Image$$ARM_LIB_STACK$$ZI$$Limit|

__Vectors       DCD     |Image$$ARM_LIB_STACK$$ZI$$Limit| ; Top of Stack
                DCD     Reset_Handler  ; Reset Handler
                DCD     NMI_Handler                         ;NMI Handler
                DCD     HardFault_Handler                   ;Hard Fault Handler
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     SVC_Handler                         ;SVCall Handler
                DCD     0                                   ;Reserved
                DCD     0                                   ;Reserved
                DCD     PendSV_Handler                      ;PendSV Handler
                DCD     SysTick_Handler                     ;SysTick Handler

                                                            ;External Interrupts
                DCD     Reserved16_IRQHandler               ;Reserved interrupt
                DCD     Reserved17_IRQHandler               ;Reserved interrupt
                DCD     Reserved18_IRQHandler               ;Reserved interrupt
                DCD     Reserved19_IRQHandler               ;Reserved interrupt
                DCD     Reserved20_IRQHandler               ;Reserved interrupt
                DCD     FTFA_IRQHandler                     ;Flash memory single interrupt vector for all sources
                DCD     LVD_LVW_IRQHandler                  ;Low-voltage detect, low-voltage warning
                DCD     PORTAE_IRQHandler                   ;Pin detect (Port A, E)
                DCD     LPI2C0_IRQHandler                   ;Inter-integrated circuit 0 interrupt
                DCD     Reserved25_IRQHandler               ;Reserved interrupt
                DCD     LPSPI0_IRQHandler                   ;Serial peripheral Interface 0 interrupt
                DCD     Reserved27_IRQHandler               ;Reserved interrupt
                DCD     LPUART0_IRQHandler                  ;Single interrupt vector for all sources
                DCD     LPUART1_IRQHandler                  ;Single interrupt vector for all sources
                DCD     LPUART2_IRQHandler                  ;Single interrupt vector for all sources
                DCD     ADC0_IRQHandler                     ;ADC0 conversion complete interrupt
                DCD     CMP0_IRQHandler                     ;CMP0 interrupt
                DCD     FTM0_IRQHandler                     ;FTM0 single interrupt vector for all sources
                DCD     FTM1_IRQHandler                     ;FTM1 single interrupt vector for all sources
                DCD     Reserved35_IRQHandler               ;Reserved interrupt
                DCD     RTC_IRQHandler                      ;Single interrupt vector for all sources
                DCD     Reserved37_IRQHandler               ;Reserved interrupt
                DCD     LPIT0_IRQHandler                    ;LPIT channel 0-1
                DCD     Reserved39_IRQHandler               ;Reserved interrupt
                DCD     TSI_IRQHandler                      ;TSI interrupt
                DCD     PDB0_IRQHandler                     ;Programmable delay block interrupt
                DCD     PORTBCD_IRQHandler                  ;Pin detect (Port B, C, D)
                DCD     SCG_RCM_IRQHandler                  ;Single interrupt vector for SCG and RCM
                DCD     WDOG_EWM_IRQHandler                 ;Single interrupt vector for WDOG and EWM
                DCD     PWT_LPTMR0_IRQHandler               ;Single interrupt vector for PWT and LPTMR0
                DCD     MSCAN_Rx_IRQHandler                 ;MSCAN Rx Interrupt
                DCD     MSCAN_ORed_IRQHandler               ;MSCAN Tx, Err and Wake-up interrupt
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors
	
				AREA    |.text|, CODE, READONLY

; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main

                CPSID   I               ; Mask interrupts
                LDR     R0, =0xE000ED08
                LDR     R1, =__Vectors
                STR     R1, [R0]
                LDR     R2, [R1]
                MSR     MSP, R2
                CPSIE   i               ; Unmask interrupts
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)
NMI_Handler\
                PROC
                EXPORT  NMI_Handler         [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler\
                PROC
                EXPORT  SVC_Handler         [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler         [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler         [WEAK]
                B       .
                ENDP
LPI2C0_IRQHandler\
                PROC
                EXPORT  LPI2C0_IRQHandler         [WEAK]
                LDR     R0, =LPI2C0_DriverIRQHandler
                BX      R0
                ENDP

LPSPI0_IRQHandler\
                PROC
                EXPORT  LPSPI0_IRQHandler         [WEAK]
                LDR     R0, =LPSPI0_DriverIRQHandler
                BX      R0
                ENDP

LPUART0_IRQHandler\
                PROC
                EXPORT  LPUART0_IRQHandler         [WEAK]
                LDR     R0, =LPUART0_DriverIRQHandler
                BX      R0
                ENDP

LPUART1_IRQHandler\
                PROC
                EXPORT  LPUART1_IRQHandler         [WEAK]
                LDR     R0, =LPUART1_DriverIRQHandler
                BX      R0
                ENDP

LPUART2_IRQHandler\
                PROC
                EXPORT  LPUART2_IRQHandler         [WEAK]
                LDR     R0, =LPUART2_DriverIRQHandler
                BX      R0
                ENDP

MSCAN_Rx_IRQHandler\
                PROC
                EXPORT  MSCAN_Rx_IRQHandler         [WEAK]
                LDR     R0, =MSCAN_DriverIRQHandler
                BX      R0
                ENDP

MSCAN_ORed_IRQHandler\
                PROC
                EXPORT  MSCAN_ORed_IRQHandler         [WEAK]
                LDR     R0, =MSCAN_DriverIRQHandler
                BX      R0
                ENDP

Default_Handler\
                PROC
                EXPORT  Reserved16_IRQHandler         [WEAK]
                EXPORT  Reserved17_IRQHandler         [WEAK]
                EXPORT  Reserved18_IRQHandler         [WEAK]
                EXPORT  Reserved19_IRQHandler         [WEAK]
                EXPORT  Reserved20_IRQHandler         [WEAK]
                EXPORT  FTFA_IRQHandler         [WEAK]
                EXPORT  LVD_LVW_IRQHandler         [WEAK]
                EXPORT  PORTAE_IRQHandler         [WEAK]
                EXPORT  LPI2C0_DriverIRQHandler         [WEAK]
                EXPORT  Reserved25_IRQHandler         [WEAK]
                EXPORT  LPSPI0_DriverIRQHandler         [WEAK]
                EXPORT  Reserved27_IRQHandler         [WEAK]
                EXPORT  LPUART0_DriverIRQHandler         [WEAK]
                EXPORT  LPUART1_DriverIRQHandler         [WEAK]
                EXPORT  LPUART2_DriverIRQHandler         [WEAK]
                EXPORT  ADC0_IRQHandler         [WEAK]
                EXPORT  CMP0_IRQHandler         [WEAK]
                EXPORT  FTM0_IRQHandler         [WEAK]
                EXPORT  FTM1_IRQHandler         [WEAK]
                EXPORT  Reserved35_IRQHandler         [WEAK]
                EXPORT  RTC_IRQHandler         [WEAK]
                EXPORT  Reserved37_IRQHandler         [WEAK]
                EXPORT  LPIT0_IRQHandler         [WEAK]
                EXPORT  Reserved39_IRQHandler         [WEAK]
                EXPORT  TSI_IRQHandler         [WEAK]
                EXPORT  PDB0_IRQHandler         [WEAK]
                EXPORT  PORTBCD_IRQHandler         [WEAK]
                EXPORT  SCG_RCM_IRQHandler         [WEAK]
                EXPORT  WDOG_EWM_IRQHandler         [WEAK]
                EXPORT  PWT_LPTMR0_IRQHandler         [WEAK]
                EXPORT  MSCAN_DriverIRQHandler         [WEAK]
                EXPORT  DefaultISR         [WEAK]
Reserved16_IRQHandler
Reserved17_IRQHandler
Reserved18_IRQHandler
Reserved19_IRQHandler
Reserved20_IRQHandler
FTFA_IRQHandler
LVD_LVW_IRQHandler
PORTAE_IRQHandler
LPI2C0_DriverIRQHandler
Reserved25_IRQHandler
LPSPI0_DriverIRQHandler
Reserved27_IRQHandler
LPUART0_DriverIRQHandler
LPUART1_DriverIRQHandler
LPUART2_DriverIRQHandler
ADC0_IRQHandler
CMP0_IRQHandler
FTM0_IRQHandler
FTM1_IRQHandler
Reserved35_IRQHandler
RTC_IRQHandler
Reserved37_IRQHandler
LPIT0_IRQHandler
Reserved39_IRQHandler
TSI_IRQHandler
PDB0_IRQHandler
PORTBCD_IRQHandler
SCG_RCM_IRQHandler
WDOG_EWM_IRQHandler
PWT_LPTMR0_IRQHandler
MSCAN_DriverIRQHandler
DefaultISR
                LDR    R0, =DefaultISR
                BX     R0
                ENDP
                  ALIGN


                END
