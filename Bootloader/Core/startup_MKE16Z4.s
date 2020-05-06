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

; <h> Flash Configuration
;   <i> 16-byte flash configuration field that stores default protection settings (loaded on reset)
;   <i> and security information that allows the MCU to restrict access to the FTFL module.
;   <h> Backdoor Comparison Key
;     <o0>  Backdoor Comparison Key 0.  <0x0-0xFF:2>
;     <o1>  Backdoor Comparison Key 1.  <0x0-0xFF:2>
;     <o2>  Backdoor Comparison Key 2.  <0x0-0xFF:2>
;     <o3>  Backdoor Comparison Key 3.  <0x0-0xFF:2>
;     <o4>  Backdoor Comparison Key 4.  <0x0-0xFF:2>
;     <o5>  Backdoor Comparison Key 5.  <0x0-0xFF:2>
;     <o6>  Backdoor Comparison Key 6.  <0x0-0xFF:2>
;     <o7>  Backdoor Comparison Key 7.  <0x0-0xFF:2>
BackDoorK0      EQU     0xFF
BackDoorK1      EQU     0xFF
BackDoorK2      EQU     0xFF
BackDoorK3      EQU     0xFF
BackDoorK4      EQU     0xFF
BackDoorK5      EQU     0xFF
BackDoorK6      EQU     0xFF
BackDoorK7      EQU     0xFF
;   </h>
;   <h> Program flash protection bytes (FPROT)
;     <i> Each program flash region can be protected from program and erase operation by setting the associated PROT bit.
;     <i> Each bit protects a 1/32 region of the program flash memory.
;     <h> FPROT0
;       <i> Program Flash Region Protect Register 0
;       <i> 1/32 - 8/32 region
;       <o.0>   FPROT0.0
;       <o.1>   FPROT0.1
;       <o.2>   FPROT0.2
;       <o.3>   FPROT0.3
;       <o.4>   FPROT0.4
;       <o.5>   FPROT0.5
;       <o.6>   FPROT0.6
;       <o.7>   FPROT0.7
nFPROT0         EQU     0x00
FPROT0          EQU     nFPROT0:EOR:0xFF
;     </h>
;     <h> FPROT1
;       <i> Program Flash Region Protect Register 1
;       <i> 9/32 - 16/32 region
;       <o.0>   FPROT1.0
;       <o.1>   FPROT1.1
;       <o.2>   FPROT1.2
;       <o.3>   FPROT1.3
;       <o.4>   FPROT1.4
;       <o.5>   FPROT1.5
;       <o.6>   FPROT1.6
;       <o.7>   FPROT1.7
nFPROT1         EQU     0x00
FPROT1          EQU     nFPROT1:EOR:0xFF
;     </h>
;     <h> FPROT2
;       <i> Program Flash Region Protect Register 2
;       <i> 17/32 - 24/32 region
;       <o.0>   FPROT2.0
;       <o.1>   FPROT2.1
;       <o.2>   FPROT2.2
;       <o.3>   FPROT2.3
;       <o.4>   FPROT2.4
;       <o.5>   FPROT2.5
;       <o.6>   FPROT2.6
;       <o.7>   FPROT2.7
nFPROT2         EQU     0x00
FPROT2          EQU     nFPROT2:EOR:0xFF
;     </h>
;     <h> FPROT3
;       <i> Program Flash Region Protect Register 3
;       <i> 25/32 - 32/32 region
;       <o.0>   FPROT3.0
;       <o.1>   FPROT3.1
;       <o.2>   FPROT3.2
;       <o.3>   FPROT3.3
;       <o.4>   FPROT3.4
;       <o.5>   FPROT3.5
;       <o.6>   FPROT3.6
;       <o.7>   FPROT3.7
nFPROT3         EQU     0x00
FPROT3          EQU     nFPROT3:EOR:0xFF
;     </h>
;   </h>
;   <h> Flash nonvolatile option byte (FOPT)
;     <i> Allows the user to customize the operation of the MCU at boot time.
;     <o.0> LPBOOT
;       <0=> Low-power boot
;       <1=> Normal boot
;     <o.2> NMI_DIS
;       <0=> NMI interrupts are always blocked
;       <1=> NMI_b pin/interrupts reset default to enabled
;     <o.3> RESET_PIN_CFG
;       <0=> RESET pin is disabled following a POR and cannot be enabled as reset function
;       <1=> RESET_b pin is dedicated
FOPT          EQU     0xFB
;   </h>
;   <h> Flash security byte (FSEC)
;     <i> WARNING: If SEC field is configured as "MCU security status is secure" and MEEN field is configured as "Mass erase is disabled",
;     <i> MCU's security status cannot be set back to unsecure state since Mass erase via the debugger is blocked !!!
;     <o.0..1> SEC
;       <2=> MCU security status is unsecure
;       <3=> MCU security status is secure
;         <i> Flash Security
;     <o.2..3> FSLACC
;       <2=> Freescale factory access denied
;       <3=> Freescale factory access granted
;         <i> Freescale Failure Analysis Access Code
;     <o.4..5> MEEN
;       <2=> Mass erase is disabled
;       <3=> Mass erase is enabled
;     <o.6..7> KEYEN
;       <2=> Backdoor key access enabled
;       <3=> Backdoor key access disabled
;         <i> Backdoor Key Security Enable
FSEC          EQU     0xFE
;   </h>
; </h>
                IF      :LNOT::DEF:RAM_TARGET
                AREA    FlashConfig, DATA, READONLY
__FlashConfig
                DCB     BackDoorK0, BackDoorK1, BackDoorK2, BackDoorK3
                DCB     BackDoorK4, BackDoorK5, BackDoorK6, BackDoorK7
                DCB     FPROT0    , FPROT1    , FPROT2    , FPROT3
                DCB     FSEC      , FOPT      , 0xFF      , 0xFF
                ENDIF


                AREA    |.text|, CODE, READONLY

; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemSetup
                IMPORT  __main

                IF      :LNOT::DEF:RAM_TARGET
                REQUIRE FlashConfig
                ENDIF

                CPSID   I               ; Mask interrupts
				;IF      :LNOT::DEF:VECTOR_MATO_FLASH
                IF		:LNOT::DEF:VECTOR_MATO_RAM	;如果未定义VECTOR_MATO_RAM这个宏,则编译以下代码
				LDR     R0, =0xE000ED08	;将立即数0xE000ed08放入到R0中
                LDR     R1, =__Vectors	;将__Vectors放入R1中
                STR     R1, [R0]		;将R1的值放入到R0所在的地址
                LDR     R2, [R1]		;从R1的地址处加载值到R2
                MSR     MSP, R2			;将R2的值写入到MSP中,即将PC指针指向栈顶
				ENDIF
                LDR     R0, =SystemSetup
                BLX     R0
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
