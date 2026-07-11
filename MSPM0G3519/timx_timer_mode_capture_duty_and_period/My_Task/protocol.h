/**
 * @file    protocol.h
 * @brief   串口协议解析 — 双字节命令状态机
 */

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** 测量模式枚举 */
typedef enum {
    MODE_NONE = 0,
    MODE_NO_DISTORTION   = 0x11,  /* 无失真        */
    MODE_BOTTOM_CLIP     = 0x22,  /* 底部失真       */
    MODE_BOTH_CLIP       = 0x33,  /* 双向失真       */
    MODE_TOP_CLIP        = 0x44,  /* 顶部失真       */
    MODE_CROSSOVER       = 0x55,  /* 交越失真       */
    MODE_SPECTRUM        = 0x66,  /* 频谱图绘制     */
    MODE_REPEAT_LAST     = 0x77,  /* 上一次失真类型 */
} MeasureMode_t;

/** 当前测量模式 */
extern volatile MeasureMode_t g_mode;

/**
 * @brief  获取失真类型名称字符串
 */
const char *Protocol_DistName(MeasureMode_t mode);

/**
 * @brief  喂入一个接收字节，状态机自动解析命令
 * @param  byte  串口收到的字节
 */
void Protocol_ParseByte(uint8_t byte);

/**
 * @brief  继电器引脚初始化（k1/k2 默认拉低）
 */
void Relay_Init(void);

/**
 * @brief  更新继电器输出（根据模式设置对应引脚）
 * @param  mode  当前测量模式
 */
void Relay_Update(MeasureMode_t mode);

/**
 * @brief  执行当前模式对应动作（在任务中周期调用）
 */
void Protocol_Execute(void);

/**
 * @brief  发送待处理的继电器响应（在 UART_Proc 中调用）
 */
void Protocol_FlushRelayResponse(void);

#ifdef __cplusplus
}
#endif

#endif /* __PROTOCOL_H */
