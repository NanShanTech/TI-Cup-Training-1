/**
 * @file    bsp.h
 * @brief   板级支持包 — MSPM0G3519 系统硬件抽象
 *
 *   BSP_Init() 封装 SysConfig 生成的硬件初始化，
 *   并提供时钟/复位/设备ID/延时等板级 API。
 */

#ifndef __BSP_H
#define __BSP_H

#include <stdint.h>

/* ---- BSP 子模块统一聚合 ---- */
#include "BSP/delay/delay.h"
#include "BSP/uart/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
 * 系统 API
 *===========================================================================*/

/** 板级初始化，内部调用 SYSCFG_DL_init()，应在 main 中最先调用 */
void BSP_Init(void);

/** 返回 CPU 主频 (Hz) */
uint32_t BSP_GetSystemClock(void);

/** 软件复位 MCU */
void BSP_Reset(void);

/** 读取芯片 DID0 寄存器 */
uint32_t BSP_GetDeviceId(void);

/*===========================================================================
 * 延时 API
 *===========================================================================*/
void BSP_DelayUs(uint32_t us);
void BSP_DelayMs(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H */
