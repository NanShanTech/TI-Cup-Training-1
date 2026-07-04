/**
 * @file    fft_analyzer.c
 * @brief   FFT 频谱分析实现（f32，标准 CMSIS-DSP arm_cfft_f32）
 */

#include "fft_analyzer.h"
#include "arm_const_structs.h"
#include <math.h>
#include <string.h>

static const arm_cfft_instance_f32 *fft_get_instance(uint16_t fft_n) {
    switch (fft_n) {
        case 128:  return &arm_cfft_sR_f32_len128;
        case 256:  return &arm_cfft_sR_f32_len256;
        case 512:  return &arm_cfft_sR_f32_len512;
        case 1024: return &arm_cfft_sR_f32_len1024;
        default:   return &arm_cfft_sR_f32_len256;
    }
}

void fft_prepare(const uint16_t *adc_data, fftin_t *out) {
    float dc_offset = 0.0f;
    const float v_scale = 3.3f / 4095.0f;

    for (uint32_t i = 0; i < FFT_N; i++) {
        dc_offset += (float)adc_data[i];
    }
    dc_offset /= (float)FFT_N;

    for (uint32_t i = 0; i < FFT_N; i++) {
        out->cmp[2 * i]     = ((float)adc_data[i] - dc_offset) * v_scale;
        out->cmp[2 * i + 1] = 0.0f;
    }
}

void fft_window(fftin_t *data, const float *window_func) {
    for (uint16_t i = 0; i < FFT_N; i++) {
        data->cmp[2 * i] *= window_func[i];
    }
}

void fft_process(fftin_t *data, fftout_t *output) {
    arm_cfft_f32(fft_get_instance(FFT_N), data->cmp, 0, 1);

    for (uint16_t i = 0; i < FFT_N_2; i++) {
        float real = data->cmp[2 * i];
        float imag = data->cmp[2 * i + 1];
        float mag;
        arm_sqrt_f32(real * real + imag * imag, &mag);
        output->mag[i]   = mag;
        output->phase[i] = atan2f(imag, real);
    }

    freqaxis_t *axis = fft_get_axis();
    for (uint32_t i = 0; i < FFT_N_2; i++) {
        if (axis->axis[i] <= DC_SCOPE) {
            output->mag[i] = 0.0f;
        }
    }
}

void fft_normalize(fftout_t *result, float norm_val) {
    float max_mag = 0.0f;
    for (uint16_t i = 0; i < FFT_N_2; i++) {
        if (result->mag[i] > max_mag) max_mag = result->mag[i];
    }
    if (max_mag < MIN_VALID_DENOM) return;
    for (uint16_t i = 0; i < FFT_N_2; i++) {
        result->mag[i] = result->mag[i] / max_mag * norm_val;
    }
}

freqaxis_t *fft_get_axis(void) {
    static freqaxis_t axis;
    static uint8_t inited = 0;
    if (!inited) {
        for (uint32_t i = 0; i < FFT_N_2; i++) {
            axis.axis[i] = (float)i * FREQ_S / FFT_N;
        }
        inited = 1;
    }
    return &axis;
}

void fft_find_peaks(const fftout_t *mag, peak3_t *peaks) {
    freqaxis_t *axis = fft_get_axis();
    memset(peaks, 0, sizeof(peak3_t));

    float max_val = 0.0f;
    for (uint16_t i = 0; i < FFT_N_2; i++) {
        if (mag->mag[i] > max_val) {
            max_val = mag->mag[i];
            peaks->index[0] = i;
        }
    }

    float max1_val = mag->mag[peaks->index[0]];
    float f1_up   = axis->axis[peaks->index[0]] + FREQ_SCOPE;
    float f1_down = axis->axis[peaks->index[0]] - FREQ_SCOPE;
    max_val = 0.0f;
    for (uint16_t i = 0; i < FFT_N_2; i++) {
        if (mag->mag[i] <= max_val || mag->mag[i] >= max1_val) continue;
        if (axis->axis[i] > f1_up || axis->axis[i] < f1_down) {
            max_val = mag->mag[i];
            peaks->index[1] = i;
        }
    }

    float max2_val = mag->mag[peaks->index[1]];
    float f2_up   = axis->axis[peaks->index[1]] + FREQ_SCOPE;
    float f2_down = axis->axis[peaks->index[1]] - FREQ_SCOPE;
    max_val = 0.0f;
    for (uint16_t i = 0; i < FFT_N_2; i++) {
        if (mag->mag[i] <= max_val || mag->mag[i] >= max2_val) continue;
        if ((axis->axis[i] > f1_up || axis->axis[i] < f1_down) &&
            (axis->axis[i] > f2_up || axis->axis[i] < f2_down)) {
            max_val = mag->mag[i];
            peaks->index[2] = i;
        }
    }
}

float fft_round_freq(float raw_freq) {
    if (raw_freq < MIN_VALID_DENOM) return 0.0f;

    uint32_t fi = (uint32_t)raw_freq;
    uint32_t up   = FREQ_STEP - (fi % (uint32_t)FREQ_STEP);
    uint32_t down = fi % (uint32_t)FREQ_STEP;
    float result  = (up >= down) ? (float)(fi - down) : (float)(fi + up);

    if (result > FREQ_LIMIT_HIGH) return FREQ_LIMIT_HIGH;
    if (result < FREQ_LIMIT_LOW)  return FREQ_LIMIT_LOW;
    return result;
}

static inline float parabolic_interpolate(float y_m1, float y_m, float y_p1) {
    float denominator = y_m1 - 2.0f * y_m + y_p1;
    if (fabsf(denominator) < 1e-6f) return y_m;
    float delta = 0.5f * (y_m1 - y_p1) / denominator;
    return y_m - 0.25f * (y_m1 - y_p1) * delta;
}

float find_vpp(const fftin_t *input) {
    uint32_t max_idx = 1, min_idx = 1;
    float discrete_max = input->cmp[2];
    float discrete_min = input->cmp[2];

    for (uint32_t i = 1; i < FFT_N - 1; i++) {
        float v = input->cmp[2 * i];
        if (v > discrete_max) { discrete_max = v; max_idx = i; }
        if (v < discrete_min) { discrete_min = v; min_idx = i; }
    }
    float true_max = parabolic_interpolate(
        input->cmp[2 * (max_idx - 1)],
        input->cmp[2 * max_idx],
        input->cmp[2 * (max_idx + 1)]);

    float true_min = parabolic_interpolate(
        input->cmp[2 * (min_idx - 1)],
        input->cmp[2 * min_idx],
        input->cmp[2 * (min_idx + 1)]);

    return true_max - true_min;
}

void fft_phase_atan(const float *complex_data, uint32_t index, float *phase) {
    *phase = atan2f(complex_data[2 * index + 1], complex_data[2 * index]);
}

float fft_max_harmonic(const float *mag, uint16_t base_idx, uint8_t harmonic_n) {
    uint32_t target = harmonic_n * base_idx;
    if (target >= FFT_N_2) return 0.0f;

    float max_v = mag[target];
    for (int off = -2; off <= 2; off++) {
        int idx = (int)target + off;
        if (idx >= 0 && idx < FFT_N_2 && mag[idx] > max_v) {
            max_v = mag[idx];
        }
    }
    return max_v;
}

WaveType_t fft_rec_wavetype(const fftout_t *mag, uint16_t base_idx) {
    if (base_idx >= FFT_N_2 || base_idx == 0) return WAVE_UNKNOWN;

    float base_amp = mag->mag[base_idx];
    if (base_amp < 0.02f) return WAVE_SINE;

    float h3_amp = fft_max_harmonic(mag->mag, base_idx, 3);
    float ratio  = h3_amp / base_amp;

    if (ratio < 0.15f)       return WAVE_SINE;
    else if (ratio < 0.40f)  return WAVE_TRIANGLE;
    else                     return WAVE_SQUARE;
}
