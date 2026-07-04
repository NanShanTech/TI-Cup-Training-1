/**
 * @file    scheduler.c
 * @brief   时间触发任务调度器实现
 *
 *   基于 g_ms_tick（SysTick_Handler 中递增）。
 *   级联计数器 + 余数保留，避免累计误差。
 *   显式注册的回调优先于 __weak 默认实现。
 */

#include "scheduler.h"
#include <string.h>

/* ---- 全局变量 ---- */
volatile uint32_t g_ms_tick = 0;
SchedTimer g_sched;
static uint32_t s_last_tick;

/* ---- 注册回调槽位 ---- */
static SchedTask_1ms_t   s_cb_1ms   = NULL;
static SchedTask_10ms_t  s_cb_10ms  = NULL;
static SchedTask_100ms_t s_cb_100ms = NULL;
static SchedTask_1sec_t  s_cb_1sec  = NULL;
static SchedTask_1min_t  s_cb_1min  = NULL;

/* ---- 注册 API ---- */
void Scheduler_RegTask_1ms  (SchedTask_1ms_t   cb) { s_cb_1ms   = cb; }
void Scheduler_RegTask_10ms (SchedTask_10ms_t  cb) { s_cb_10ms  = cb; }
void Scheduler_RegTask_100ms(SchedTask_100ms_t cb) { s_cb_100ms = cb; }
void Scheduler_RegTask_1sec (SchedTask_1sec_t  cb) { s_cb_1sec  = cb; }
void Scheduler_RegTask_1min (SchedTask_1min_t  cb) { s_cb_1min  = cb; }

/* ---- 获取当前 1ms Tick ---- */
static uint32_t Tick_Get(void) {
    return g_ms_tick;
}

/* ---- 计算流逝 Tick 数（处理 32 位溢出）---- */
static uint32_t Tick_Elapsed(uint32_t last) {
    uint32_t now = Tick_Get();
    if (now >= last)
        return now - last;
    return (0xFFFFFFFFU - last) + now + 1U;
}

void Scheduler_Init(void) {
    memset(&g_sched, 0, sizeof(g_sched));
    s_last_tick = Tick_Get();
}

void Scheduler_Run(void) {
    uint32_t elapsed = Tick_Elapsed(s_last_tick);
    if (elapsed == 0) return;

    s_last_tick += elapsed;

    /* ---- 1ms ---- */
    g_sched.ms1 += elapsed;
    if (s_cb_1ms) s_cb_1ms(elapsed);
    else           Task_1ms(elapsed);

    if (g_sched.ms1 < NUM_1MS_FOR_10MS) return;

    /* ---- 10ms ---- */
    {
        uint32_t ticks = g_sched.ms1;
        g_sched.ms10 += ticks / NUM_1MS_FOR_10MS;
        g_sched.ms1  %= NUM_1MS_FOR_10MS;
        if (s_cb_10ms) s_cb_10ms(ticks);
        else           Task_10ms(ticks);
    }

    if (g_sched.ms10 < NUM_10MS_FOR_100MS) return;

    /* ---- 100ms ---- */
    g_sched.ms100 += g_sched.ms10 / NUM_10MS_FOR_100MS;
    g_sched.ms10  %= NUM_10MS_FOR_100MS;
    if (s_cb_100ms) s_cb_100ms();
    else            Task_100ms();

    if (g_sched.ms100 < NUM_100MS_FOR_1SEC) return;

    /* ---- 1 秒 ---- */
    {
        uint32_t sec_add = g_sched.ms100 / NUM_100MS_FOR_1SEC;
        g_sched.ms100   %= NUM_100MS_FOR_1SEC;
        g_sched.sec1    += sec_add;
        if (s_cb_1sec) s_cb_1sec();
        else           Task_1sec();
    }

    if (g_sched.sec1 < NUM_1SEC_FOR_1MIN) return;

    /* ---- 1 分钟 ---- */
    g_sched.min1 += g_sched.sec1 / NUM_1SEC_FOR_1MIN;
    g_sched.sec1 %= NUM_1SEC_FOR_1MIN;
    if (s_cb_1min) s_cb_1min();
    else           Task_1min();
}

/* ---- 默认空实现（用户按需覆盖）---- */
__attribute__((weak)) void Task_1ms  (uint32_t ticks) { (void)ticks; }
__attribute__((weak)) void Task_10ms (uint32_t ticks) { (void)ticks; }
__attribute__((weak)) void Task_100ms(void)            { }
__attribute__((weak)) void Task_1sec (void)            { }
__attribute__((weak)) void Task_1min (void)            { }
