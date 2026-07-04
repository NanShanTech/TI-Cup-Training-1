/**
 * @file    fft_analyzer_q15.c
 * @brief   q15 定点 FFT 频谱分析实现（arm_cfft_q15）
 */

#include "fft_analyzer_q15.h"
#include "arm_const_structs.h"
#include <math.h>
#include <string.h>

#define PAI 3.14159265358979323846f

static const arm_cfft_instance_q15 *fft_q15_get_instance(uint16_t fft_n) {
    switch (fft_n) {
        case 256:  return &arm_cfft_sR_q15_len256;
        case 512:  return &arm_cfft_sR_q15_len512;
        case 1024: return &arm_cfft_sR_q15_len1024;
        case 2048: return &arm_cfft_sR_q15_len2048;
        case 4096: return &arm_cfft_sR_q15_len4096;
        default:   return &arm_cfft_sR_q15_len256;
    }
}

void fft_q15_prepare(const uint16_t *adc_data, fftin_q15_t *out) {
    int32_t dc_offset = 0;

    for (uint32_t i = 0; i < FFT_Q15_N; i++) {
        dc_offset += (int32_t)adc_data[i];
    }
    dc_offset /= (int32_t)FFT_Q15_N;

    for (uint32_t i = 0; i < FFT_Q15_N; i++) {
        int32_t val = ((int32_t)adc_data[i] - dc_offset) << FFT_Q15_SCALE_SHIFT;
        if (val > 32767)  val = 32767;
        if (val < -32768) val = -32768;
        out->cmp[2 * i]     = (int16_t)val;
        out->cmp[2 * i + 1] = 0;
    }
}

void fft_q15_window(fftin_q15_t *data, const float *window_func) {
    for (uint16_t i = 0; i < FFT_Q15_N; i++) {
        float val = (float)data->cmp[2 * i] * window_func[i];
        if (val > 32767.0f)  val = 32767.0f;
        if (val < -32768.0f) val = -32768.0f;
        data->cmp[2 * i] = (int16_t)val;
    }
}

void fft_q15_process(fftin_q15_t *data, fftout_q15_t *output) {
    arm_cfft_q15(fft_q15_get_instance(FFT_Q15_N), data->cmp, 0, 1);
    arm_cmplx_mag_q15(data->cmp, output->mag, FFT_Q15_N);

    for (uint16_t i = 0; i < FFT_Q15_N_2; i++) {
        output->mag[i] <<= 1;
    }
    output->mag[0] = 0;

    freqaxis_q15_t *axis = fft_q15_get_axis();
    for (uint32_t i = 0; i < FFT_Q15_N_2; i++) {
        if (axis->axis[i] <= DC_SCOPE) {
            output->mag[i] = 0;
        }
    }
}

freqaxis_q15_t *fft_q15_get_axis(void) {
    static freqaxis_q15_t axis;
    static uint8_t inited = 0;
    if (!inited) {
        for (uint32_t i = 0; i < FFT_Q15_N_2; i++) {
            axis.axis[i] = (float)i * FFT_Q15_SAMPLE_RATE / FFT_Q15_N;
        }
        inited = 1;
    }
    return &axis;
}

void fft_q15_find_peaks(const fftout_q15_t *mag, peak3_t *peaks) {
    freqaxis_q15_t *axis = fft_q15_get_axis();
    memset(peaks, 0, sizeof(peak3_t));

    {
        q15_t max_val;
        uint32_t idx;
        arm_max_q15(mag->mag, FFT_Q15_N_2, &max_val, &idx);
        peaks->index[0] = (uint16_t)idx;
    }

    {
        int16_t max1_val = mag->mag[peaks->index[0]];
        float f1_up   = axis->axis[peaks->index[0]] + FREQ_SCOPE;
        float f1_down = axis->axis[peaks->index[0]] - FREQ_SCOPE;
        int16_t max_val = 0;
        for (uint16_t i = 0; i < FFT_Q15_N_2; i++) {
            if (mag->mag[i] <= max_val || mag->mag[i] >= max1_val) continue;
            if (axis->axis[i] > f1_up || axis->axis[i] < f1_down) {
                max_val = mag->mag[i];
                peaks->index[1] = i;
            }
        }
    }

    {
        int16_t max2_val = mag->mag[peaks->index[1]];
        float f1_up   = axis->axis[peaks->index[0]] + FREQ_SCOPE;
        float f1_down = axis->axis[peaks->index[0]] - FREQ_SCOPE;
        float f2_up   = axis->axis[peaks->index[1]] + FREQ_SCOPE;
        float f2_down = axis->axis[peaks->index[1]] - FREQ_SCOPE;
        int16_t max_val = 0;
        for (uint16_t i = 0; i < FFT_Q15_N_2; i++) {
            if (mag->mag[i] <= max_val || mag->mag[i] >= max2_val) continue;
            if ((axis->axis[i] > f1_up || axis->axis[i] < f1_down) &&
                (axis->axis[i] > f2_up || axis->axis[i] < f2_down)) {
                max_val = mag->mag[i];
                peaks->index[2] = i;
            }
        }
    }
}

float fft_q15_round_freq(float raw_freq) {
    if (raw_freq < MIN_VALID_DENOM) return 0.0f;

    uint32_t fi     = (uint32_t)raw_freq;
    uint32_t up     = FREQ_STEP - (fi % (uint32_t)FREQ_STEP);
    uint32_t down   = fi % (uint32_t)FREQ_STEP;
    float    result = (up >= down) ? (float)(fi - down) : (float)(fi + up);

    if (result > FREQ_LIMIT_HIGH) return FREQ_LIMIT_HIGH;
    if (result < FREQ_LIMIT_LOW)  return FREQ_LIMIT_LOW;
    return result;
}

float fft_q15_phase(const int16_t *fft_input, uint16_t index) {
    int32_t idx2  = (int32_t)index * 2;
    float   f_imag = (float)fft_input[idx2 + 1] / 32768.0f;
    float   f_real = (float)fft_input[idx2]     / 32768.0f;
    float   radians = atan2f(f_imag, f_real);
    return radians * (180.0f / PAI);
}

int16_t fft_q15_max_harmonic(const int16_t *mag, uint16_t base_idx, uint8_t harmonic_n) {
    uint32_t target = (uint32_t)harmonic_n * base_idx;
    if (target >= FFT_Q15_N_2) return 0;

    int16_t max_v = mag[target];
    for (int off = -2; off <= 2; off++) {
        int idx = (int)target + off;
        if (idx >= 0 && idx < (int)FFT_Q15_N_2 && mag[idx] > max_v) {
            max_v = mag[idx];
        }
    }
    return max_v;
}

WaveType_t fft_q15_rec_wavetype(const fftout_q15_t *mag, uint16_t base_idx) {
    if (base_idx >= FFT_Q15_N_2 || base_idx == 0) return WAVE_UNKNOWN;

    int16_t base_amp = mag->mag[base_idx];
    if (base_amp < 10) return WAVE_SINE;

    int16_t h3_amp = fft_q15_max_harmonic(mag->mag, base_idx, 3);
    if (base_amp == 0) return WAVE_UNKNOWN;

    float ratio = (float)h3_amp / (float)base_amp;

    if (ratio < 0.15f)       return WAVE_SINE;
    else if (ratio < 0.40f)  return WAVE_TRIANGLE;
    else                     return WAVE_SQUARE;
}
