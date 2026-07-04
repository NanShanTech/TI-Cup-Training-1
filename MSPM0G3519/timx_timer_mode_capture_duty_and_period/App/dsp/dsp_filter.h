/**
 * @file    dsp_filter.h
 * @brief   数字滤波器模块 —— FIR / IIR Biquad CMSIS-DSP 薄包装
 *
 *   所有 state 缓冲由调用方提供。滤波器系数来自 MATLAB 设计脚本。
 */

#ifndef __DSP_FILTER_H
#define __DSP_FILTER_H

#include "arm_math.h"

/*===========================================================================
 * 一、FIR 滤波器（直接型，浮点）
 *===========================================================================*/

typedef struct {
    arm_fir_instance_f32 inst;
} DspFilter_FIR;

int  DspFilter_FIR_Init(DspFilter_FIR  *flt,
                        uint16_t        num_taps,
                        const float32_t *coeffs,
                        float32_t       *state,
                        uint32_t        block_size);

void DspFilter_FIR_Process(DspFilter_FIR   *flt,
                           const float32_t *src,
                           float32_t       *dst,
                           uint32_t         block_size);

/*===========================================================================
 * 二、IIR Biquad 级联滤波器（DF2T，浮点）
 *===========================================================================*/

typedef struct {
    arm_biquad_cascade_df2T_instance_f32 inst;
} DspFilter_Biquad;

int  DspFilter_Biquad_Init(DspFilter_Biquad *flt,
                           uint8_t           num_stages,
                           const float32_t  *coeffs,
                           float32_t        *state);

void DspFilter_Biquad_Process(DspFilter_Biquad *flt,
                              const float32_t  *src,
                              float32_t        *dst,
                              uint32_t          block_size);

/*===========================================================================
 * 三、内置滤波器系数预设
 *===========================================================================*/

/* FIR: 5 点滑动平均 */
#define DSP_FLT_FIR_MA5_TAPS  5U
extern const float32_t dsp_flt_fir_ma5_coeffs[DSP_FLT_FIR_MA5_TAPS];

/* FIR: 占位模板（粘贴 MATLAB fir1/firpm 输出） */
#define DSP_FLT_FIR_TEMPLATE_TAPS  8U
extern const float32_t dsp_flt_fir_template_coeffs[DSP_FLT_FIR_TEMPLATE_TAPS];

/* IIR Biquad: 2 阶 Butterworth 低通 fc=2kHz */
#define DSP_FLT_BIQUAD_LP_BUTTER2_2KHZ_STAGES  1U
extern const float32_t dsp_flt_biquad_lp_butter2_2khz_coeffs[5U * DSP_FLT_BIQUAD_LP_BUTTER2_2KHZ_STAGES];

/* IIR Biquad: 占位模板 */
#define DSP_FLT_BIQUAD_TEMPLATE_STAGES  2U
extern const float32_t dsp_flt_biquad_template_coeffs[5U * DSP_FLT_BIQUAD_TEMPLATE_STAGES];

#endif /* __DSP_FILTER_H */
