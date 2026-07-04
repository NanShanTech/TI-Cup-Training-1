/**
 * @file    tasks.h
 * @brief   应用任务函数声明 — MSPM0G3519 定时器捕获版本
 *
 *   将定时器捕获分析、串口上报等业务封装为独立任务函数。
 */

#ifndef __TASKS_H
#define __TASKS_H

#include <stdint.h>
#include "App/app_types.h"

/* ---- 全局波形分析结果（供各任务共享）---- */
extern Wave_Struct g_wave_info;

/* ---- 任务函数 ---- */

/** ADC处理 +（ 10ms 周期） */
void ADC_Proc(uint32_t ticks);

/** 串口结果输出任务（ 100ms 周期） */
void UART_Proc(void);

#endif /* __TASKS_H */
