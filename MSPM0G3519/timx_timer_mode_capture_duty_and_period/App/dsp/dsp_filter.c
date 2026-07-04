/**
 * @file    dsp_filter.c
 * @brief   数字滤波器实现 + 系数库
 */

#include "dsp_filter.h"

/*===========================================================================
 * FIR 系数
 *===========================================================================*/

/* 5 点滑动平均：h = ones(1,5)/5 */
const float32_t dsp_flt_fir_ma5_coeffs[DSP_FLT_FIR_MA5_TAPS] = {
    0.2f, 0.2f, 0.2f, 0.2f, 0.2f
};

/* FIR 占位模板：默认单位冲激响应（全通） */
const float32_t dsp_flt_fir_template_coeffs[DSP_FLT_FIR_TEMPLATE_TAPS] = {
    /* === 在此粘贴 MATLAB fir1/firpm 输出 === */
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f
};

/*===========================================================================
 * IIR Biquad 系数
 *===========================================================================*/

/* 2 阶 Butterworth 低通，fc=2 kHz
 * MATLAB:
 *   Fs = 8000; fc = 2000;
 *   [b,a] = butter(2, fc/(Fs/2)); [sos,g] = tf2sos(b,a);
 */
const float32_t dsp_flt_biquad_lp_butter2_2khz_coeffs[5U * DSP_FLT_BIQUAD_LP_BUTTER2_2KHZ_STAGES] = {
    /* {b0, b1, b2, a1, a2} */
    0.2928932188f, 0.5857864376f, 0.2928932188f, 0.0f, -0.1715728753f
};

/* Biquad 占位模板：默认全通 */
const float32_t dsp_flt_biquad_template_coeffs[5U * DSP_FLT_BIQUAD_TEMPLATE_STAGES] = {
    /* === 在此粘贴 MATLAB tf2sos 输出 === */
    /* stage 0 */ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    /* stage 1 */ 1.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

/*===========================================================================
 * FIR 实现（薄包装 arm_fir_f32）
 *===========================================================================*/

int DspFilter_FIR_Init(DspFilter_FIR  *flt,
                       uint16_t        num_taps,
                       const float32_t *coeffs,
                       float32_t       *state,
                       uint32_t        block_size)
{
    if ((flt == NULL) || (coeffs == NULL) || (state == NULL) ||
        (num_taps == 0U) || (block_size == 0U)) {
        return -1;
    }
    arm_fir_init_f32(&flt->inst, num_taps,
                     (float32_t *)coeffs, state, block_size);
    return 0;
}

void DspFilter_FIR_Process(DspFilter_FIR   *flt,
                           const float32_t *src,
                           float32_t       *dst,
                           uint32_t         block_size)
{
    if ((flt == NULL) || (src == NULL) || (dst == NULL) || (block_size == 0U)) {
        return;
    }
    arm_fir_f32(&flt->inst, (float32_t *)src, dst, block_size);
}

/*===========================================================================
 * IIR Biquad 实现（薄包装 arm_biquad_cascade_df2T_f32）
 *===========================================================================*/

int DspFilter_Biquad_Init(DspFilter_Biquad *flt,
                          uint8_t           num_stages,
                          const float32_t  *coeffs,
                          float32_t        *state)
{
    if ((flt == NULL) || (coeffs == NULL) || (state == NULL) ||
        (num_stages == 0U)) {
        return -1;
    }
    arm_biquad_cascade_df2T_init_f32(&flt->inst, num_stages,
                                     (float32_t *)coeffs, state);
    return 0;
}

void DspFilter_Biquad_Process(DspFilter_Biquad *flt,
                              const float32_t  *src,
                              float32_t        *dst,
                              uint32_t          block_size)
{
    if ((flt == NULL) || (src == NULL) || (dst == NULL) || (block_size == 0U)) {
        return;
    }
    arm_biquad_cascade_df2T_f32(&flt->inst, (float32_t *)src, dst, block_size);
}
