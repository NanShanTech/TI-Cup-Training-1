/**
 * @file    thd.h
 * @brief   THD 总谐波失真计算 — 三 bin 能量求和
 */

#ifndef __THD_H
#define __THD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  计算谐波幅度（峰值相邻三 bin 能量开方）
 * @param  mag        幅度谱
 * @param  base_idx   基频索引
 * @param  harmonic   谐波次数 (1=基频, 2~5=谐波)
 * @return 谐波幅度 (三 bin RMS)
 */
float THD_HarmonicEnergy(const float *mag, uint16_t base_idx,
                         uint8_t harmonic);

/**
 * @brief  计算 THD（前五次谐波，三 bin 法）
 * @param  mag       幅度谱
 * @param  base_idx  基频索引
 * @return THD 百分比 (0.0 ~ 100.0+)，失败返回 -1.0f
 */
float THD_Compute(const float *mag, uint16_t base_idx);

#ifdef __cplusplus
}
#endif

#endif /* __THD_H */
