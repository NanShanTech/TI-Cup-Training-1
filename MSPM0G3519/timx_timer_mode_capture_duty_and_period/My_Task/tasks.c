/**
 * @file    tasks.c
 * @brief   任务实现 — f32 FFT 2048 点 + THD 前五次谐波分析
 *          夏子杰 2026.7.5
 */

#include "BSP/bsp.h"
#include "App/app.h"
#include "App/app_config.h"
#include "App/fft/fft_analyzer.h"
#include "thd.h"
#include "tasks.h"
#include "Hanning_Window_2048.h"
#include <math.h>
#include <string.h>

extern uint16_t g_adc_buffer[FFT_N];

Wave_Struct g_wave_info;

/* FFT 工作缓冲区 — f32 类型，体积大放静态区 */
static fftin_t     s_fft_in;
static fftout_t    s_fft_out;
static peak3_t     s_peaks;
static freqaxis_t *s_axis;


/* ---- 10ms 周期：等待 ADC 完成 → FFT → THD → 重启动 ---- */
void ADC_Proc(uint32_t ticks)
{
    (void)ticks;

    if (!ADC_IsCaptureDone()) {
        return;
    }
    ADC_ClearCaptureDone();
// for(int i =0;i<FFT_N;i++)
// {
//     Serial_Printf("ch1:%d\r\n",g_adc_buffer[i]);
// }
    /* f32 FFT：去直流 → CFFT → 求模 → 找峰 */
    fft_prepare(g_adc_buffer, &s_fft_in);
    fft_window(&s_fft_in,Hanning_Window_2048);
    fft_process(&s_fft_in, &s_fft_out);

    fft_find_peaks(&s_fft_out, &s_peaks);
    s_axis = fft_get_axis();

    uint16_t base_idx = s_peaks.index[0];
    float    raw_freq = s_axis->axis[base_idx];

    /* 前五次谐波与基波幅度之比（五 bin 能量法） */
    float v1 = THD_HarmonicEnergy(s_fft_out.mag, base_idx, 1);
    g_wave_info.HarmRatio[1] = 100.0f;
    for (uint8_t h = 2; h <= 5; h++) {
        float vh = THD_HarmonicEnergy(s_fft_out.mag, base_idx, h);
        g_wave_info.HarmRatio[h] = (v1 > 1e-9f) ? (vh / v1 * 100.0f) : 0.0f;
    }
    g_wave_info.THD = THD_Compute(s_fft_out.mag, base_idx);

    /* 清空 buffer，开启新一轮采集 */
    memset(g_adc_buffer, 0, sizeof(g_adc_buffer));
    ADC_StartNormal(g_adc_buffer, FFT_N);
}

/* ---- 1 秒周期：串口上报 ---- */
void UART_Proc(void)
{
    Serial_Printf(
        "THD:%.1f%% H2:%.1f%% H3:%.1f%% H4:%.1f%% H5:%.1f%%\r\n",
        g_wave_info.THD,
        g_wave_info.HarmRatio[2],
        g_wave_info.HarmRatio[3],
        g_wave_info.HarmRatio[4],
        g_wave_info.HarmRatio[5]);
}
