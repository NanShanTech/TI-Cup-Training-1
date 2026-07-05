/**
 * @file    uart.h
 * @brief   单串口驱动 — TX 输出 + RX 空闲中断 + 环形缓冲区
 *          参考 STM32H7 HAL_UARTEx_ReceiveToIdle_IT
 */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 串口发送 ---- */
void Serial_Init(void);
void Serial_Send(const uint8_t *data, uint16_t len);
void Serial_Printf(const char *format, ...);

/* ---- 环形缓冲区接收（空闲中断驱动）---- */
void     Serial_RxInit(void);
bool     Serial_RxAvailable(void);
uint8_t  Serial_RxRead(void);
uint16_t Serial_RxReadAll(uint8_t *buf, uint16_t maxLen);

/* ISR 辅助（在 main.c 的中断处理函数中调用） */
void     Serial_RxPushByte(uint8_t byte);
bool     Serial_RxFifoReadByte(uint8_t *byte);
void     Serial_RxClearFlags(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_UART_H */
