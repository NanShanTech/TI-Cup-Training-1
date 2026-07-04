/**
 * @file    scheduler.h
 * @brief   时间触发任务调度器（基于 SysTick 1ms 级联）
 *
 *   时间链：1ms → 10ms → 100ms → 1秒 → 1分钟
 *
 *   两种用法：
 *     1. 显式注册（推荐）：Scheduler_RegTask_10ms(ADC_Proc)
 *     2. 覆盖 weak 函数（兼容旧方式）：重定义 Task_10ms()
 */

#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include <stdint.h>

/* ---- 时间片配置 ---- */
#define NUM_1MS_FOR_10MS    10U
#define NUM_10MS_FOR_100MS  10U
#define NUM_100MS_FOR_1SEC  10U
#define NUM_1SEC_FOR_1MIN   60U

/* ---- 全局 ms 计数器（SysTick_Handler 中递增）---- */
extern volatile uint32_t g_ms_tick;

/* ---- 级联 Tick 计数器 ---- */
typedef struct {
    uint32_t ms1;
    uint32_t ms10;
    uint32_t ms100;
    uint32_t sec1;
    uint32_t min1;
} SchedTimer;

extern SchedTimer g_sched;

/* ---- 任务回调函数类型 ---- */
typedef void (*SchedTask_1ms_t)  (uint32_t ticks);
typedef void (*SchedTask_10ms_t) (uint32_t ticks);
typedef void (*SchedTask_100ms_t)(void);
typedef void (*SchedTask_1sec_t) (void);
typedef void (*SchedTask_1min_t) (void);

/* ---- 调度器 API ---- */
void Scheduler_Init(void);
void Scheduler_Run (void);

/* ---- 任务注册 API（显式注册优先于 weak 覆盖）---- */
void Scheduler_RegTask_1ms  (SchedTask_1ms_t   cb);
void Scheduler_RegTask_10ms (SchedTask_10ms_t  cb);
void Scheduler_RegTask_100ms(SchedTask_100ms_t cb);
void Scheduler_RegTask_1sec (SchedTask_1sec_t  cb);
void Scheduler_RegTask_1min (SchedTask_1min_t  cb);

/* ---- 弱函数声明（保留兼容）---- */
void Task_1ms  (uint32_t ticks);
void Task_10ms (uint32_t ticks);
void Task_100ms(void);
void Task_1sec (void);
void Task_1min (void);

#endif
