/**
 * @file    uart.h
 * @brief   UART 驱动 — XDS 调试串口 + HMI 触摸屏串口
 */

#ifndef __BSP_UART_H
#define __BSP_UART_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- XDS 调试串口（USB 转 UART，连 PC）---- */
void Serial_Init(void);
void Serial_Send(const uint8_t *data, uint16_t len);
void Serial_Printf(const char *format, ...);

/* ---- HMI 触摸屏串口 ---- */
void HMI_Serial_Init(void);
void HMI_Serial_Send(const uint8_t *data, uint16_t len);
void HMI_Serial_Printf(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_UART_H */
