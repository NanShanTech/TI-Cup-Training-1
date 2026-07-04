/**
 * @file    dsp_adaptive.c
 * @brief   自适应滤波器实现 — LMS / NLMS / RLS
 */

#include "dsp_adaptive.h"
#include <string.h>

/*===========================================================================
 * LMS 实现
 *===========================================================================*/

void DspAdaptive_LMS_Init(DspAdaptive_LMS *lms,
                          uint16_t         num_taps,
                          float32_t       *coeffs,
                          float32_t       *state,
                          float32_t        mu,
                          uint32_t         block_size)
{
    if ((lms == NULL) || (coeffs == NULL) || (state == NULL)) return;
    arm_lms_init_f32(&lms->inst, num_taps, coeffs, state, mu, block_size);
}

void DspAdaptive_LMS_Process(DspAdaptive_LMS *lms,
                             const float32_t *src,
                             float32_t       *ref,
                             float32_t       *out,
                             float32_t       *err,
                             uint32_t         block_size)
{
    if ((lms == NULL) || (src == NULL) || (ref == NULL) ||
        (out == NULL) || (err == NULL)) return;
    arm_lms_f32(&lms->inst, (float32_t *)src, ref, out, err, block_size);
}

/*===========================================================================
 * NLMS 实现
 *===========================================================================*/

void DspAdaptive_NLMS_Init(DspAdaptive_NLMS *nlms,
                           uint16_t          num_taps,
                           float32_t        *coeffs,
                           float32_t        *state,
                           float32_t         mu,
                           float32_t        *energy,
                           float32_t        *x0,
                           uint32_t          block_size)
{
    if ((nlms == NULL) || (coeffs == NULL) || (state == NULL) ||
        (energy == NULL) || (x0 == NULL)) return;
    arm_lms_norm_init_f32(&nlms->inst, num_taps, coeffs, state, mu, block_size);
    memset(energy, 0, block_size * sizeof(float32_t));
    *x0 = 0.0f;
}

void DspAdaptive_NLMS_Process(DspAdaptive_NLMS *nlms,
                              const float32_t  *src,
                              float32_t        *ref,
                              float32_t        *out,
                              float32_t        *err,
                              uint32_t          block_size)
{
    if ((nlms == NULL) || (src == NULL) || (ref == NULL) ||
        (out == NULL) || (err == NULL)) return;
    arm_lms_norm_f32(&nlms->inst, (float32_t *)src, ref, out, err, block_size);
}

/*===========================================================================
 * RLS 实现（自实现 + CMSIS-DSP 矩阵运算）
 *===========================================================================*/

int DspAdaptive_RLS_Init(DspAdaptive_RLS *rls,
                         uint16_t          taps,
                         float             lambda,
                         float             delta,
                         float            *w,
                         float            *P,
                         float            *x_buf,
                         float            *scratch)
{
    if ((rls == NULL) || (w == NULL) || (P == NULL) ||
        (x_buf == NULL) || (scratch == NULL) || (taps == 0U)) {
        return -1;
    }

    rls->taps   = taps;
    rls->lambda = lambda;
    rls->delta  = delta;
    rls->w      = w;
    rls->P      = P;
    rls->x_buf  = x_buf;
    rls->scratch= scratch;

    memset(w,       0, (size_t)taps      * sizeof(float));
    memset(x_buf,   0, (size_t)taps      * sizeof(float));
    memset(scratch, 0, (size_t)(2 * taps) * sizeof(float));

    const float p_init = 1.0f / delta;
    const uint32_t n = (uint32_t)taps;
    for (uint32_t i = 0U; i < n * n; i++) P[i] = 0.0f;
    for (uint32_t i = 0U; i < n; i++) P[i * n + i] = p_init;

    rls->inited = 1U;
    return 0;
}

void DspAdaptive_RLS_Reset(DspAdaptive_RLS *rls)
{
    if ((rls == NULL) || (rls->inited == 0U)) return;

    const uint32_t n = (uint32_t)rls->taps;
    memset(rls->w,       0, n * sizeof(float));
    memset(rls->x_buf,   0, n * sizeof(float));
    memset(rls->scratch, 0, (size_t)(2U * n) * sizeof(float));

    const float p_init = 1.0f / rls->delta;
    for (uint32_t i = 0U; i < n * n; i++) rls->P[i] = 0.0f;
    for (uint32_t i = 0U; i < n; i++) rls->P[i * n + i] = p_init;
}

void DspAdaptive_RLS_Update(DspAdaptive_RLS *rls,
                            float             x,
                            float             d,
                            float            *y,
                            float            *e)
{
    if ((rls == NULL) || (rls->inited == 0U) || (y == NULL) || (e == NULL)) return;

    const uint32_t N = (uint32_t)rls->taps;
    const float    lam = rls->lambda;
    float *const   w   = rls->w;
    float *const   P   = rls->P;
    float *const   x_buf = rls->x_buf;
    float *const   pi  = rls->scratch;
    float *const   k   = rls->scratch + N;

    /* 步骤 1: 更新输入延迟线 */
    for (uint32_t i = N - 1U; i > 0U; i--) x_buf[i] = x_buf[i - 1U];
    x_buf[0U] = x;

    /* 步骤 2: pi = P * x_buf */
    {
        arm_matrix_instance_f32 P_mat, x_vec, pi_vec;
        arm_mat_init_f32(&P_mat,  (uint16_t)N, (uint16_t)N, P);
        arm_mat_init_f32(&x_vec,  (uint16_t)N, 1U,            x_buf);
        arm_mat_init_f32(&pi_vec, (uint16_t)N, 1U,            pi);
        arm_mat_mult_f32(&P_mat, &x_vec, &pi_vec);
    }

    /* 步骤 3: gamma = lambda + x_buf^T * pi */
    float gamma;
    arm_dot_prod_f32(x_buf, pi, N, &gamma);
    gamma += lam;
    if (gamma < 1e-8f) gamma = 1e-8f;

    /* 步骤 4: k = pi / gamma */
    {
        const float inv_gamma = 1.0f / gamma;
        arm_scale_f32(pi, inv_gamma, k, N);
    }

    /* 步骤 5: y = w^T * x_buf */
    float y_val;
    arm_dot_prod_f32(w, x_buf, N, &y_val);
    *y = y_val;

    /* 步骤 6: e = d - y */
    *e = d - y_val;

    /* 步骤 7: w = w + k * e */
    for (uint32_t i = 0U; i < N; i++) w[i] += k[i] * (*e);

    /* 步骤 8: P = (P - k * pi^T) / lambda */
    const float inv_lam = 1.0f / lam;
    for (uint32_t i = 0U; i < N; i++) {
        const float ki = k[i];
        for (uint32_t j = 0U; j < N; j++) {
            P[i * N + j] = (P[i * N + j] - ki * pi[j]) * inv_lam;
        }
    }
}
