/**
 * @file    tasks.c
 * @brief   任务实现 — FFT + THD + 串口协议解析
 *          夏子杰 2026.7.5
 */

#include "BSP/bsp.h"
#include "App/app.h"
#include "App/app_config.h"
#include "App/fft/fft_analyzer.h"
#include "thd.h"
#include "protocol.h"
#include "tasks.h"
#include "Hanning_Window_2048.h"
#include <math.h>
#include <string.h>

extern uint16_t g_adc_buffer[FFT_N];

Wave_Struct g_wave_info;

static fftin_t     s_fft_in;
static fftout_t    s_fft_out;
static peak3_t     s_peaks;
static freqaxis_t *s_axis;


/* ---- 10ms 周期：ADC → FFT → THD → 重启动 ---- */
void ADC_Proc(uint32_t ticks)
{
    (void)ticks;

    /* ADC DMA 采集完成后处理 */
    if (!ADC_IsCaptureDone()) {
        return;
    }
    ADC_ClearCaptureDone();

    fft_prepare(g_adc_buffer, &s_fft_in);
    fft_window(&s_fft_in, Hanning_Window_2048);
    fft_process(&s_fft_in, &s_fft_out);
    fft_find_peaks(&s_fft_out, &s_peaks);
    s_axis = fft_get_axis();

    uint16_t base_idx = s_peaks.index[0];
    float    raw_freq = s_axis->axis[base_idx];

    float v1 = THD_HarmonicEnergy(s_fft_out.mag, base_idx, 1);
    g_wave_info.HarmRatio[1] = 100.0f;
    for (uint8_t h = 2; h <= 5; h++) {
        float vh = THD_HarmonicEnergy(s_fft_out.mag, base_idx, h);
        g_wave_info.HarmRatio[h] = (v1 > 1e-9f) ? (vh / v1 * 100.0f) : 0.0f;
    }
    g_wave_info.THD = THD_Compute(s_fft_out.mag, base_idx);

    memset(g_adc_buffer, 0, sizeof(g_adc_buffer));
    ADC_StartNormal(g_adc_buffer, FFT_N);

   
}

/* ---- 1 秒周期：接收解析 + 上报 HMI---- */
void UART_Proc(void)
{ 
    Serial_Printf("t1.txt=\"%.2f%%\"\xff\xff\xff", (double)g_wave_info.THD);
    Serial_Printf("t3.txt=\"%s\"\xff\xff\xff", Protocol_DistName(g_mode));

    if (g_mode == MODE_SPECTRUM) {
        Serial_Printf("t5.txt=\"%.1f%%\"\xff\xff\xff",
                      (double)g_wave_info.HarmRatio[2]);
        Serial_Printf("t8.txt=\"%.1f%%\"\xff\xff\xff",
                      (double)g_wave_info.HarmRatio[3]);
        Serial_Printf("t10.txt=\"%.1f%%\"\xff\xff\xff",
                      (double)g_wave_info.HarmRatio[4]);
        Serial_Printf("t12.txt=\"%.1f%%\"\xff\xff\xff",
                      (double)g_wave_info.HarmRatio[5]);

        /* 频谱柱状图 — 左基波 → 右五次谐波，基波高度 3/4 屏 */
        {
            uint8_t data[300];
            int bar_w = 2, gap = 67, margin = 10;
            int pos = 0;

            /* 左边缘留白 */
            for (int i = 0; i < margin && pos < 300; i++) {
                data[pos++] = 0;
            }
            /* 屏幕先发的在右边 → 逆序填充: H5→H4→H3→H2→H1 */
            for (int h = 5; h >= 1; h--) {
                int height = (int)(g_wave_info.HarmRatio[h] / 100.0f * 150.0f);
                if (height > 200) height = 200;
                for (int i = 0; i < bar_w && pos < 300; i++) {
                    data[pos++] = (uint8_t)height;
                }
                if (h > 1) {
                    for (int i = 0; i < gap && pos < 300; i++) {
                        data[pos++] = 0;
                    }
                }
            }
            /* 右边缘留白 */
            while (pos < 300) {
                data[pos++] = 0;
            }

            Serial_Printf("addt s0.id,0,300\xff\xff\xff");
            for (int i = 0; i < 300; i++) {
                Serial_Send(&data[i], 1);
            }
        }
    }

    /* 接收 — 从环形缓冲区解析协议命令 */
    uint8_t buf[64];
    uint16_t len = Serial_RxReadAll(buf, sizeof(buf));
    for (uint16_t i = 0; i < len; i++) {
        Protocol_ParseByte(buf[i]);
    }

    /* 发送继电器翻转响应 */
    Protocol_FlushRelayResponse();

    /* 清空环形缓冲区残余 */
    while (Serial_RxAvailable()) {
        Serial_RxRead();
    }
}
