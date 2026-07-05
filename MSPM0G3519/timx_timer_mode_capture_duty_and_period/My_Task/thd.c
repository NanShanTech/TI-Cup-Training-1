/**
 * @file    thd.c
 * @brief   THD 总谐波失真 — 三 bin 能量求和，抗频谱泄漏
 *         夏子杰 2026.7.5
 */

#include "thd.h"
#include "App/app_config.h"
#include <math.h>

/* 在谐波频率附近 (±2 bin) 找幅度峰，返回峰值索引 */
static uint16_t thd_find_peak_idx(const float *mag, uint32_t target)
{
    uint16_t peak_idx = (uint16_t)target;
    float    peak_val = mag[target];

    for (int off = -2; off <= 2; off++) {
        int idx = (int)target + off;
        if (idx > 0 && idx < FFT_N_2 - 1 && mag[idx] > peak_val) {
            peak_val = mag[idx];
            peak_idx = (uint16_t)idx;
        }
    }
    return peak_idx;
}

float THD_HarmonicEnergy(const float *mag, uint16_t base_idx,
                         uint8_t harmonic)
{
    if (harmonic == 0 || harmonic > 5) {
        return 0.0f;
    }

    uint32_t target = (uint32_t)harmonic * base_idx;
    if (target >= (uint32_t)(FFT_N_2 - 1)) {
        return 0.0f;
    }

    uint16_t peak = thd_find_peak_idx(mag, target);

    /* 峰值左右各取 2 个 bin，五 bin 能量开方 */
    float sum_sq = 0.0f;
    for (int off = -2; off <= 2; off++) {
        int idx = (int)peak + off;
        if (idx > 0 && idx < FFT_N_2) {
            sum_sq += mag[idx] * mag[idx];
        }
    }

    return sqrtf(sum_sq);
}

float THD_Compute(const float *mag, uint16_t base_idx)
{
    float v1 = THD_HarmonicEnergy(mag, base_idx, 1);
    if (v1 < 1e-9f) {
        return -1.0f;
    }

    float sum_sq = 0.0f;
    for (uint8_t h = 2; h <= 5; h++) {
        float vh = THD_HarmonicEnergy(mag, base_idx, h);
        sum_sq += vh * vh;
    }

    return sqrtf(sum_sq) / v1 * 100.0f;
}
