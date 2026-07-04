/**
 * @file    fft_analyzer_q15.h
 * @brief   q15 定点 FFT 频谱分析 API（arm_cfft_q15）
 */

#ifndef __FFT_ANALYZER_Q15_H
#define __FFT_ANALYZER_Q15_H

#include "App/app_types.h"
#include "arm_math.h"

void fft_q15_prepare(const uint16_t *adc_data, fftin_q15_t *out);
void fft_q15_window(fftin_q15_t *data, const float *window_func);
void fft_q15_process(fftin_q15_t *data, fftout_q15_t *output);
freqaxis_q15_t *fft_q15_get_axis(void);
void fft_q15_find_peaks(const fftout_q15_t *mag, peak3_t *peaks);
float fft_q15_round_freq(float raw_freq);
WaveType_t fft_q15_rec_wavetype(const fftout_q15_t *mag, uint16_t base_idx);
int16_t fft_q15_max_harmonic(const int16_t *mag, uint16_t base_idx, uint8_t harmonic_n);
float fft_q15_phase(const int16_t *fft_input, uint16_t index);

#endif /* __FFT_ANALYZER_Q15_H */
