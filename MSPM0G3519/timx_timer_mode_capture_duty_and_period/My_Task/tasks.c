/**
 * @file    tasks.c
 * @brief   任务的具体实现
            夏子杰 2026.7.5
 */

#include "BSP/bsp.h"
#include "App/app.h"
#include "App/app_config.h"
#include "tasks.h"

#include <math.h>
#include <string.h>

extern uint16_t g_adc_buffer[FFT_Q15_N];

Wave_Struct g_wave_info;

/* FFT 工作缓冲区（体积大，放静态区，不放栈） */
static fftin_q15_t  s_fft_in;
static fftout_q15_t s_fft_out;
static float        s_window[FFT_Q15_N];
static bool         s_window_inited = false;


/* ---- 10ms 周期：等待 ADC 完成 → FFT → 清 buffer → 重启动 ---- */
void ADC_Proc(uint32_t ticks)
{
    (void)ticks;

    if (!ADC_IsCaptureDone()) {
        return;
    }
    ADC_ClearCaptureDone();

    /* q15 FFT：去直流 → 加窗 → CFFT → 求模 */

    fft_q15_prepare(g_adc_buffer, &s_fft_in);
    // fft_q15_window(&s_fft_in, s_window);
    fft_q15_process(&s_fft_in, &s_fft_out);
    
    /* 清空 buffer，开启新一轮采集 */
    memset(g_adc_buffer, 0, sizeof(g_adc_buffer));
    ADC_StartNormal(g_adc_buffer, FFT_Q15_N);
}

/* ---- 1 秒周期：串口上报 ---- */
void UART_Proc(void)
{
    Serial_Printf("Freq: %.1f Hz  Vpp: %.2f V\r\n", g_wave_info.Freq, g_wave_info.Vpp);
}
