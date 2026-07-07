/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G351X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G351X
#define CONFIG_MSPM0G3519

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define GPIO_HFXT_PORT                                                     GPIOA
#define GPIO_HFXIN_PIN                                             DL_GPIO_PIN_5
#define GPIO_HFXIN_IOMUX                                         (IOMUX_PINCM10)
#define GPIO_HFXOUT_PIN                                            DL_GPIO_PIN_6
#define GPIO_HFXOUT_IOMUX                                        (IOMUX_PINCM11)
#define CPUCLK_FREQ                                                     80000000
/* Defines for SYSPLL_ERR_01 Workaround */
/* Represent 1.000 as 1000 */
#define FLOAT_TO_INT_SCALE                                               (1000U)
#define FCC_EXPECTED_RATIO                                                  2500
#define FCC_UPPER_BOUND                       (FCC_EXPECTED_RATIO * (1 + 0.003))
#define FCC_LOWER_BOUND                       (FCC_EXPECTED_RATIO * (1 - 0.003))

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);


/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMA0)
#define TIMER_0_INST_IRQHandler                                 TIMA0_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                          (1249U)
#define TIMER_0_INST_PUB_0_CH                                                (1)



/* Defines for HMI */
#define HMI_INST                                                           UART0
#define HMI_INST_FREQUENCY                                              40000000
#define HMI_INST_IRQHandler                                     UART0_IRQHandler
#define HMI_INST_INT_IRQN                                         UART0_INT_IRQn
#define GPIO_HMI_RX_PORT                                                   GPIOA
#define GPIO_HMI_TX_PORT                                                   GPIOA
#define GPIO_HMI_RX_PIN                                           DL_GPIO_PIN_11
#define GPIO_HMI_TX_PIN                                           DL_GPIO_PIN_10
#define GPIO_HMI_IOMUX_RX                                        (IOMUX_PINCM22)
#define GPIO_HMI_IOMUX_TX                                        (IOMUX_PINCM21)
#define GPIO_HMI_IOMUX_RX_FUNC                         IOMUX_PINCM22_PF_UART0_RX
#define GPIO_HMI_IOMUX_TX_FUNC                         IOMUX_PINCM21_PF_UART0_TX
#define HMI_BAUD_RATE                                                     (9600)
#define HMI_IBRD_40_MHZ_9600_BAUD                                          (260)
#define HMI_FBRD_40_MHZ_9600_BAUD                                           (27)





/* Defines for ADC12_0 */
#define ADC12_0_INST                                                        ADC1
#define ADC12_0_INST_IRQHandler                                  ADC1_IRQHandler
#define ADC12_0_INST_INT_IRQN                                    (ADC1_INT_IRQn)
#define ADC12_0_ADCMEM_0                                      DL_ADC12_MEM_IDX_0
#define ADC12_0_ADCMEM_0_REF                DL_ADC12_REFERENCE_VOLTAGE_VDDA_VSSA
#define ADC12_0_INST_SUB_CH                                                  (1)
#define GPIO_ADC12_0_C0_PORT                                               GPIOA
#define GPIO_ADC12_0_C0_PIN                                       DL_GPIO_PIN_15
#define GPIO_ADC12_0_IOMUX_C0                                    (IOMUX_PINCM37)
#define GPIO_ADC12_0_IOMUX_C0_FUNC                (IOMUX_PINCM37_PF_UNCONNECTED)



/* Defines for DMA_CH2 */
#define DMA_CH2_CHAN_ID                                                      (0)
#define ADC12_0_INST_DMA_TRIGGER                      (DMA_ADC1_EVT_GEN_BD_TRIG)


/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOB)

/* Defines for PIN_0: GPIOB.1 with pinCMx 13 on package pin 21 */
#define LED_PIN_0_PIN                                            (DL_GPIO_PIN_1)
#define LED_PIN_0_IOMUX                                          (IOMUX_PINCM13)
/* Port definition for Pin Group PIN1 */
#define PIN1_PORT                                                        (GPIOC)

/* Defines for PIN_1: GPIOC.16 with pinCMx 69 on package pin 35 */
#define PIN1_PIN_1_PIN                                          (DL_GPIO_PIN_16)
#define PIN1_PIN_1_IOMUX                                         (IOMUX_PINCM69)
/* Port definition for Pin Group PIN2 */
#define PIN2_PORT                                                        (GPIOC)

/* Defines for PIN_2: GPIOC.17 with pinCMx 70 on package pin 36 */
#define PIN2_PIN_2_PIN                                          (DL_GPIO_PIN_17)
#define PIN2_PIN_2_IOMUX                                         (IOMUX_PINCM70)
/* Port definition for Pin Group PIN3 */
#define PIN3_PORT                                                        (GPIOC)

/* Defines for PIN_3: GPIOC.15 with pinCMx 64 on package pin 11 */
#define PIN3_PIN_3_PIN                                          (DL_GPIO_PIN_15)
#define PIN3_PIN_3_IOMUX                                         (IOMUX_PINCM64)
/* Port definition for Pin Group PIN4 */
#define PIN4_PORT                                                        (GPIOC)

/* Defines for PIN_4: GPIOC.13 with pinCMx 62 on package pin 12 */
#define PIN4_PIN_4_PIN                                          (DL_GPIO_PIN_13)
#define PIN4_PIN_4_IOMUX                                         (IOMUX_PINCM62)
/* Port definition for Pin Group PIN5 */
#define PIN5_PORT                                                        (GPIOC)

/* Defines for PIN_5: GPIOC.14 with pinCMx 63 on package pin 13 */
#define PIN5_PIN_5_PIN                                          (DL_GPIO_PIN_14)
#define PIN5_PIN_5_IOMUX                                         (IOMUX_PINCM63)




/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);

bool SYSCFG_DL_SYSCTL_SYSPLL_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_HMI_init(void);
void SYSCFG_DL_ADC12_0_init(void);
void SYSCFG_DL_DMA_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
