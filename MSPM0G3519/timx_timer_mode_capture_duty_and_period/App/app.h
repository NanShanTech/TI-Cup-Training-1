/**
 * @file    app.h
 * @brief   App 层统一入口
 *
 *   参考 STM32 工程 bsp_system.h 作为主 include 的模式，
 *   所有 App 层模块通过此头文件聚合引入。
 */

#ifndef __APP_H
#define __APP_H

#include "App/app_config.h"
#include "App/app_types.h"
#include "BSP/bsp.h"
#include "Scheduler/scheduler.h"
#include "App/dsp/dsp_filter.h"
#include "App/dsp/dsp_analyze.h"
#include "App/dsp/dsp_adaptive.h"
#include "App/fft/fft_analyzer.h"
#include "App/fft/fft_analyzer_q15.h"

#endif
