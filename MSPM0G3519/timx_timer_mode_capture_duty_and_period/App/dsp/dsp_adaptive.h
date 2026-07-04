/**
 * @file    dsp_adaptive.h
 * @brief   自适应滤波器模块 — LMS / NLMS / RLS
 */

#ifndef __DSP_ADAPTIVE_H
#define __DSP_ADAPTIVE_H

#include "arm_math.h"

/*===========================================================================
 * 一、LMS 自适应滤波器
 *===========================================================================*/

typedef struct {
    arm_lms_instance_f32 inst;
} DspAdaptive_LMS;

void DspAdaptive_LMS_Init(DspAdaptive_LMS *lms,
                          uint16_t         num_taps,
                          float32_t       *coeffs,
                          float32_t       *state,
                          float32_t        mu,
                          uint32_t         block_size);

void DspAdaptive_LMS_Process(DspAdaptive_LMS *lms,
                             const float32_t *src,
                             float32_t       *ref,
                             float32_t       *out,
                             float32_t       *err,
                             uint32_t         block_size);

/*===========================================================================
 * 二、NLMS 归一化 LMS
 *===========================================================================*/

typedef struct {
    arm_lms_norm_instance_f32 inst;
} DspAdaptive_NLMS;

void DspAdaptive_NLMS_Init(DspAdaptive_NLMS *nlms,
                           uint16_t          num_taps,
                           float32_t        *coeffs,
                           float32_t        *state,
                           float32_t         mu,
                           float32_t        *energy,
                           float32_t        *x0,
                           uint32_t          block_size);

void DspAdaptive_NLMS_Process(DspAdaptive_NLMS *nlms,
                              const float32_t  *src,
                              float32_t        *ref,
                              float32_t        *out,
                              float32_t        *err,
                              uint32_t          block_size);

/*===========================================================================
 * 三、RLS 递推最小二乘（自实现 + CMSIS-DSP 矩阵运算）
 *===========================================================================*/

typedef struct {
    uint16_t   taps;
    float      lambda;
    float      delta;
    float     *w;
    float     *P;
    float     *x_buf;
    float     *scratch;
    uint8_t    inited;
} DspAdaptive_RLS;

int  DspAdaptive_RLS_Init(DspAdaptive_RLS *rls,
                          uint16_t          taps,
                          float             lambda,
                          float             delta,
                          float            *w,
                          float            *P,
                          float            *x_buf,
                          float            *scratch);

void DspAdaptive_RLS_Reset(DspAdaptive_RLS *rls);

void DspAdaptive_RLS_Update(DspAdaptive_RLS *rls,
                            float             x,
                            float             d,
                            float            *y,
                            float            *e);

#endif /* __DSP_ADAPTIVE_H */
