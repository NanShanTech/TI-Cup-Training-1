/**
 * @file    fft_analyzer.h
 * @brief   FFT 频谱分析 API（f32，标准 CMSIS-DSP arm_cfft_f32）
 */

#ifndef __FFT_ANALYZER_H
#define __FFT_ANALYZER_H

#include "App/app_types.h"
#include "arm_math.h"

void fft_prepare(const uint16_t *adc_data, fftin_t *out);
void fft_window(fftin_t *data, const float *window_func);
void fft_process(fftin_t *data, fftout_t *output);
void fft_normalize(fftout_t *result, float norm_val);
freqaxis_t *fft_get_axis(void);
void fft_find_peaks(const fftout_t *mag, peak3_t *peaks);
float fft_round_freq(float raw_freq);
WaveType_t fft_rec_wavetype(const fftout_t *mag, uint16_t base_idx);
float find_vpp(const fftin_t *input);
void fft_phase_atan(const float *complex_data, uint32_t index, float *phase);
float fft_max_harmonic(const float *mag, uint16_t base_idx, uint8_t harmonic_n);

#endif /* __FFT_ANALYZER_H */
