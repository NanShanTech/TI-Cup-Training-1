/**
 * @file    bsp.h
 * @brief   BSP 层统一入口 — 硬件初始化、系统 API、延时、采样定时器
 */

#ifndef __BSP_H
#define __BSP_H

#include <stdbool.h>
#include <stdint.h>

/* ---- BSP 子模块 ---- */
#include "BSP/adc/adc.h"
#include "BSP/uart/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * 硬件默认参数（BSP 层自有，不依赖 App 层配置）
 *===========================================================================*/
#define BSP_ADC_DEFAULT_SAMPLE_RATE  32000U   /* ADC 默认采样率 (Hz) */
#define BSP_CONSOLE_BAUDRATE         115200U  /* 调试串口波特率       */

/*===========================================================================
 * 系统 API
 *===========================================================================*/
void     BSP_Init(void);
uint32_t BSP_GetSystemClock(void);
void     BSP_Reset(void);
uint32_t BSP_GetDeviceId(void);

/*===========================================================================
 * 延时 API（基于 delay_cycles 忙等待）
 *===========================================================================*/
void BSP_DelayUs(uint32_t us);
void BSP_DelayMs(uint32_t ms);

/*===========================================================================
 * 采样定时器（TIMER_0，供 ADC 触发用）
 *===========================================================================*/
bool     SampleTimer_Init(void);
bool     SampleTimer_SetRate(uint32_t sampleRateHz);
uint32_t SampleTimer_GetRate(void);
void     SampleTimer_Start(void);
void     SampleTimer_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H */
