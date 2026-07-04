/**
 * @file    dsp_analyze.c
 * @brief   增强频谱分析实现
 *
 *   M0+ 软浮点性能警告：1024 点 RFFT 约需 50+ ms。
 */

#include "dsp_analyze.h"
#include <math.h>

/*===========================================================================
 * RFFT 实例初始化
 *===========================================================================*/

int DspAnalyze_Init(DspAnalyze *ctx, uint32_t fft_size)
{
    if (ctx == NULL) return -1;

    switch (fft_size) {
    case 128U:
    case 256U:
    case 512U:
    case 1024U:
    case 2048U:
        break;
    default:
        return -1;
    }

    arm_rfft_fast_init_f32(&ctx->rfft_inst, (uint16_t)fft_size);
    ctx->fft_size = fft_size;
    ctx->inited   = 1U;
    return 0;
}

int DspAnalyze_RFFT_Mag(DspAnalyze     *ctx,
                        const float32_t *in,
                        float32_t       *scratch,
                        float32_t       *mag)
{
    if ((ctx == NULL) || (ctx->inited == 0U) ||
        (in == NULL) || (scratch == NULL) || (mag == NULL)) {
        return -1;
    }

    const uint32_t N = ctx->fft_size;

    arm_rfft_fast_f32(&ctx->rfft_inst, (float32_t *)in, scratch, 0);
    arm_cmplx_mag_f32(scratch, mag, N / 2U);
    mag[0] = 0.0f;

    return 0;
}

void DspAnalyze_FindPeak(const float32_t *mag, uint32_t len,
                         uint32_t *peak_bin, float32_t *peak_val)
{
    if ((mag == NULL) || (peak_bin == NULL) || (peak_val == NULL)) return;
    arm_max_f32(mag, len, peak_val, peak_bin);
}

/*===========================================================================
 * 峰值频率内插
 *===========================================================================*/

float DspAnalyze_InterpFreq(const float32_t *mag, uint32_t len,
                            uint32_t peak_bin,
                            float    fs_hz, uint32_t fft_size)
{
    if ((mag == NULL) || (len < 3U) || (fft_size == 0U)) return 0.0f;

    if ((peak_bin == 0U) || (peak_bin >= len - 1U)) {
        return (float)peak_bin * fs_hz / (float)fft_size;
    }

    const float L = mag[peak_bin - 1U];
    const float P = mag[peak_bin];
    const float R = mag[peak_bin + 1U];
    const float denom = L - 2.0f * P + R;

    if (fabsf(denom) < 1e-9f) {
        return (float)peak_bin * fs_hz / (float)fft_size;
    }

    const float delta = 0.5f * (L - R) / denom;
    float clamped = delta;
    if (clamped > 0.5f)  clamped = 0.5f;
    if (clamped < -0.5f) clamped = -0.5f;

    const float interp_bin = (float)peak_bin + clamped;
    return interp_bin * fs_hz / (float)fft_size;
}

/*===========================================================================
 * RMS 有效值
 *===========================================================================*/

float DspAnalyze_RMS(const float32_t *x, uint32_t n)
{
    if ((x == NULL) || (n == 0U)) return 0.0f;
    float32_t rms;
    arm_rms_f32((float32_t *)x, n, &rms);
    return rms;
}

/*===========================================================================
 * SNR 信噪比估算（频域法）
 *===========================================================================*/

float DspAnalyze_SNR(const float32_t *mag, uint32_t len,
                     uint32_t peak_bin, uint32_t guard)
{
    if ((mag == NULL) || (len == 0U) || (peak_bin >= len)) return 0.0f;

    const float sig_pow = mag[peak_bin] * mag[peak_bin];
    if (sig_pow < 1e-12f) return 0.0f;

    float    noise_sum = 0.0f;
    uint32_t noise_cnt = 0U;
    const uint32_t lo = (peak_bin > guard) ? (peak_bin - guard) : 0U;
    const uint32_t hi = (peak_bin + guard < len) ? (peak_bin + guard) : (len - 1U);

    noise_sum += mag[0] * mag[0];
    noise_cnt++;

    for (uint32_t i = 1U; i < len; i++) {
        if (i >= lo && i <= hi) continue;
        noise_sum += mag[i] * mag[i];
        noise_cnt++;
    }

    if (noise_cnt == 0U) return 100.0f;

    const float noise_pow = noise_sum / (float)noise_cnt;
    if (noise_pow < 1e-15f) return 100.0f;

    return 10.0f * log10f(sig_pow / noise_pow);
}

/*===========================================================================
 * THD 总谐波失真
 *===========================================================================*/

float DspAnalyze_THD(const float32_t *mag, uint32_t len,
                     uint32_t peak_bin, uint32_t max_harm,
                     uint32_t fft_size)
{
    if ((mag == NULL) || (len == 0U) || (peak_bin >= len) ||
        (peak_bin == 0U) || (fft_size == 0U)) return 0.0f;

    const float A_fund = mag[peak_bin];
    if (A_fund < 1e-9f) return 0.0f;

    float harm_pow_sum = 0.0f;

    for (uint32_t h = 2U; h <= max_harm; h++) {
        uint32_t harm_bin = h * peak_bin;
        if (harm_bin >= len) break;

        float harm_max = 0.0f;
        for (int32_t d = -1; d <= 1; d++) {
            int32_t idx = (int32_t)harm_bin + d;
            if (idx >= 0 && (uint32_t)idx < len) {
                if (mag[idx] > harm_max) harm_max = mag[idx];
            }
        }
        harm_pow_sum += harm_max * harm_max;
    }

    return sqrtf(harm_pow_sum) / A_fund;
}
