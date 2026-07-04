/**
 * @file    tasks.c
 * @brief   应用任务函数实现 — ADC 电压采集版
 *
 *   ADC 通过 DMA 持续采样，DMA ISR 置位 g_adc_dma_done。
 *   UART_Proc 每秒读取最新电压并通过串口上报。
 */

#include "BSP/bsp.h"
#include "App/app.h"
#include "tasks.h"
#include "App/app_config.h"

/* ---- 外部引用 ---- */
extern volatile uint8_t g_adc_dma_done;
extern uint16_t g_adc_buffer[FFT_Q15_N];

/* ---- 全局波形分析结果 ---- */
Wave_Struct g_wave_info;

/* ---- 波形类型 → 字符串 ---- */
static const char *wave_type_str(WaveType_t t)
{
    switch (t) {
        case WAVE_SINE:     return "SINE";
        case WAVE_SQUARE:   return "SQUARE";
        case WAVE_TRIANGLE: return "TRIANGLE";
        default:            return "UNKNOWN";
    }
}

/* ================================================================
 * ADC 数据采集任务（建议 10ms 周期）
 *
 *   从 DMA 缓冲区读取 ADC 采样值，计算平均电压。
 * ================================================================ */
void ADC_Proc(uint32_t ticks)
{
    (void)ticks;

    if (!g_adc_dma_done) return;
    g_adc_dma_done = 0;

    /* 计算 DMA 缓冲区中所有采样点的平均值 */
    uint32_t sum = 0;
    for (uint16_t i = 0; i < FFT_Q15_N; i++) {
        sum += g_adc_buffer[i];
    }
    uint16_t avg = (uint16_t)(sum / FFT_Q15_N);

    /* 12-bit ADC，参考电压 VDDA ≈ 3.3V */
    g_wave_info.Vpp = avg * 3.3f / 4095.0f;
    g_wave_info.Freq = 0.0f;
    g_wave_info.Wave_type = WAVE_UNKNOWN;
}

/* ================================================================
 * 串口电压上报任务（1秒周期）
 *
 *   每秒上报一次最新电压值。
 * ================================================================ */
void UART_Proc(void)
{
    Serial_Printf("ADC Voltage: %.2f V\r\n", g_wave_info.Vpp);
}
