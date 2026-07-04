/*
 * 全局类型定义 — MSPM0G3519 定时器捕获 + 信号处理模板
 *
 *   仅包含 <stdint.h>，不引入重型头文件。
 *   所有结构体按功能分组。
 */

#ifndef __APP_TYPES_H
#define __APP_TYPES_H

#include <stdint.h>

/*===========================================================================
 * 一、波形与信号分析类型
 *===========================================================================*/

/** 波形类型枚举 */
typedef enum {
    WAVE_SINE = 0,      /* 正弦波 */
    WAVE_SQUARE,        /* 方波   */
    WAVE_TRIANGLE,      /* 三角波 */
    WAVE_UNKNOWN        /* 未知   */
} WaveType_t;

/** 波形测量结果（频率 + 峰峰值 + 波形类型） */
typedef struct {
    float       Freq;       /* 频率 (Hz)  */
    float       Vpp;        /* 峰峰值 (V) */
    WaveType_t  Wave_type;
} Wave_Struct;

/*===========================================================================
 * 二、f32 FFT 专用类型（与 arm_cfft_f32 匹配）
 *===========================================================================*/
#include "App/app_config.h"

#define FFT_2N  (FFT_N * 2)

/** FFT 时域输入（复数交错: real[0], imag[0], ...） */
typedef struct {
    float cmp[FFT_2N];
} fftin_t;

/** FFT 频域输出 */
typedef struct {
    float mag[FFT_N_2];
    float phase[FFT_N_2];
} fftout_t;

/** 前三大峰值索引 */
typedef struct {
    uint16_t index[3];
} peak3_t;

/** 频率轴 */
typedef struct {
    float axis[FFT_N_2];
} freqaxis_t;

/*===========================================================================
 * 三、q15 FFT 专用类型（与 arm_cfft_q15 匹配，可到 4096 点）
 *===========================================================================*/
#define FFT_Q15_2N  (FFT_Q15_N * 2)

/** q15 FFT 时域输入（复数交错，实部=ADC 采样值<<SCALE，虚部=0） */
typedef struct {
    int16_t cmp[FFT_Q15_2N];
} fftin_q15_t;

/** q15 FFT 频域输出 */
typedef struct {
    int16_t mag[FFT_Q15_N_2];
    float   phase[FFT_Q15_N_2];   /* 相位保持 float（atan2f 输出） */
} fftout_q15_t;

/** q15 频率轴 */
typedef struct {
    float axis[FFT_Q15_N_2];
} freqaxis_q15_t;

/*===========================================================================
 * 五、频率测量状态类型
 *===========================================================================*/

typedef enum {
    FMODE_PERIOD = 0,   /* 测周法（低频高精度） */
    FMODE_COUNT  = 1    /* 测频法（高频）       */
} FreqMode_t;

/*===========================================================================
 * 六、DSP 信号处理类型（f32，用于 FIR/IIR/RLS）
 *===========================================================================*/

/** 窗函数类型枚举 */
typedef enum {
    DSP_WIN_NONE = 0,
    DSP_WIN_HANN,
    DSP_WIN_HAMMING,
    DSP_WIN_BLACKMAN,
    DSP_WIN_BLACKMAN_HARRIS,
    DSP_WIN_NUTTALL,
    DSP_WIN_FLATTOP,
    DSP_WIN_COUNT
} DspWinType_t;

/** RLS 自适应滤波器实例 */
typedef struct {
    uint16_t   taps;
    float      lambda;
    float      delta;
    float     *w;
    float     *P;
    float     *x_buf;
    float     *scratch;
    uint8_t    inited;
} DspRLS_t;

#endif /* __APP_TYPES_H */
