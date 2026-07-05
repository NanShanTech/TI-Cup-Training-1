/**
 * @file    adc.h
 * @brief   ADC 驱动 — DMA 连续采样，中断通知完成
 */

#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ADC_Init(void);
bool ADC_StartNormal(uint16_t *buffer, uint16_t sampleCount);
void ADC_Stop(void);
bool ADC_IsCaptureDone(void);
void ADC_ClearCaptureDone(void);
void ADC_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_ADC_H */
