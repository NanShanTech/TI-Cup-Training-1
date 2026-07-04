/**
 * @file    uart.h
 * @brief   UART 串口通信模块
 */

#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>

void Serial_Init(void);
void Serial_Send(const uint8_t *data, uint16_t len);
void Serial_Printf(const char *format, ...);

/* ---- 兼容旧名称 ---- */
#define UART_Init       Serial_Init
#define UART_Printf     Serial_Printf

#endif /* __SERIAL_H */
