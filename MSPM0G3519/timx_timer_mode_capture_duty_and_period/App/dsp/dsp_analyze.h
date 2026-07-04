/**
 * @file    dsp_analyze.h
 * @brief   增强频谱分析模块 — RFFT / RMS / SNR / THD / 峰值内插
 *
 *   M0+ 单精度浮点较慢，推荐后续迁移到 q15 定点 RFFT。
 */

#ifndef __DSP_ANALYZE_H
#define __DSP_ANALYZE_H

#include "arm_math.h"
#include "App/app_config.h"

/*===========================================================================
 * 一、RFFT（实数 FFT）—— 浮点版，M0+ 慎用大点数
 *===========================================================================*/

typedef struct {
    arm_rfft_fast_instance_f32 rfft_inst;
    uint32_t                   fft_size;
    uint8_t                    inited;
} DspAnalyze;

int  DspAnalyze_Init(DspAnalyze *ctx, uint32_t fft_size);
int  DspAnalyze_RFFT_Mag(DspAnalyze     *ctx,
                         const float32_t *in,
                         float32_t       *scratch,
                         float32_t       *mag);
void DspAnalyze_FindPeak(const float32_t *mag, uint32_t len,
                         uint32_t *peak_bin, float32_t *peak_val);

/*===========================================================================
 * 二、峰值频率内插
 *===========================================================================*/

float DspAnalyze_InterpFreq(const float32_t *mag, uint32_t len,
                            uint32_t peak_bin,
                            float    fs_hz, uint32_t fft_size);

/*===========================================================================
 * 三、RMS 有效值
 *===========================================================================*/

float DspAnalyze_RMS(const float32_t *x, uint32_t n);

/*===========================================================================
 * 四、SNR 信噪比估算（频域法）
 *===========================================================================*/

float DspAnalyze_SNR(const float32_t *mag, uint32_t len,
                     uint32_t peak_bin, uint32_t guard);

/*===========================================================================
 * 五、THD 总谐波失真
 *===========================================================================*/

float DspAnalyze_THD(const float32_t *mag, uint32_t len,
                     uint32_t peak_bin, uint32_t max_harm,
                     uint32_t fft_size);

#endif /* __DSP_ANALYZE_H */
