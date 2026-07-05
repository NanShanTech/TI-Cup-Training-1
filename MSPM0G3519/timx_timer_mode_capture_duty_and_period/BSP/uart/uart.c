/**
 * @file    uart.c
 * @brief   单串口驱动 — UART0 9600bps / TX + RX 空闲中断 + 环形缓冲区
 *          参考 STM32H7 HAL_UARTEx_ReceiveToIdle_IT
 */

#include "uart.h"
#include "BSP/bsp.h"
#include "ti_msp_dl_config.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ---- 环形接收缓冲区 ---- */
#define RX_BUF_SIZE  500U

static volatile uint8_t  s_rx_buf[RX_BUF_SIZE];
static volatile uint16_t s_rx_head = 0;
static volatile uint16_t s_rx_tail = 0;

static bool s_uart_inited = false;

/* ---- 内部辅助 ---- */

static void UART_SendBlocking(UART_Regs *uart, const uint8_t *data, uint16_t len)
{
    if (data == NULL) return;
    for (uint16_t i = 0; i < len; i++) {
        DL_UART_transmitDataBlocking(uart, data[i]);
    }
}

static void UART_PrintfTo(UART_Regs *uart, const char *format, va_list args)
{
    char tmp[128];
    int  len = vsnprintf(tmp, sizeof(tmp), format, args);
    if (len <= 0) return;
    if (len >= (int)sizeof(tmp)) len = (int)sizeof(tmp) - 1;
    UART_SendBlocking(uart, (const uint8_t *)tmp, (uint16_t)len);
}

/* ---- 环形缓冲区操作 ---- */

static inline uint16_t rx_buf_next(uint16_t idx)
{
    return (idx + 1U) % RX_BUF_SIZE;
}

static inline bool rx_buf_full(void)
{
    return rx_buf_next(s_rx_head) == s_rx_tail;
}

static inline bool rx_buf_empty(void)
{
    return s_rx_head == s_rx_tail;
}

static void rx_buf_push(uint8_t byte)
{
    if (rx_buf_full()) {
        s_rx_tail = rx_buf_next(s_rx_tail);
    }
    s_rx_buf[s_rx_head] = byte;
    s_rx_head = rx_buf_next(s_rx_head);
}

/* ---- 单串口初始化（TX + RX + 空闲中断）---- */

void Serial_Init(void)
{
    if (s_uart_inited) return;

    /* 硬件初始化由 SYSCFG_DL_HMI_init() 完成。
     * RX 超时周期必须设为非零，否则 RX_TIMEOUT 中断不工作 */
    DL_UART_Extend_setRXInterruptTimeout(HMI_INST, 10);
    DL_UART_Extend_enableInterrupt(HMI_INST,
        DL_UART_EXTEND_INTERRUPT_RX_TIMEOUT_ERROR);

    s_uart_inited = true;
}

void Serial_RxInit(void)
{
    s_rx_head = 0;
    s_rx_tail = 0;
    DL_UART_Extend_clearInterruptStatus(HMI_INST,
        DL_UART_INTERRUPT_RX_TIMEOUT_ERROR);
    NVIC_ClearPendingIRQ(HMI_INST_INT_IRQN);
    NVIC_EnableIRQ(HMI_INST_INT_IRQN);
}

/* ---- 环形缓冲区读取 API ---- */

bool Serial_RxAvailable(void)
{
    return !rx_buf_empty();
}

uint8_t Serial_RxRead(void)
{
    if (rx_buf_empty()) return 0;
    uint8_t byte = s_rx_buf[s_rx_tail];
    s_rx_tail = rx_buf_next(s_rx_tail);
    return byte;
}

uint16_t Serial_RxReadAll(uint8_t *buf, uint16_t maxLen)
{
    uint16_t cnt = 0;
    while (!rx_buf_empty() && cnt < maxLen) {
        buf[cnt++] = Serial_RxRead();
    }
    return cnt;
}

/* ---- 串口发送 ---- */

void Serial_Send(const uint8_t *data, uint16_t len)
{
    if (!s_uart_inited) Serial_Init();
    UART_SendBlocking(HMI_INST, data, len);
}

void Serial_Printf(const char *format, ...)
{
    va_list argptr;
    if (!s_uart_inited) Serial_Init();
    va_start(argptr, format);
    UART_PrintfTo(HMI_INST, format, argptr);
    va_end(argptr);
}

/* ---- ISR 辅助函数（在 main.c 的中断处理函数中调用）---- */

void Serial_RxPushByte(uint8_t byte)
{
    __disable_irq();
    rx_buf_push(byte);
    __enable_irq();
}

bool Serial_RxFifoReadByte(uint8_t *byte)
{
    return DL_UART_Extend_receiveDataCheck(HMI_INST, byte);
}

void Serial_RxClearFlags(void)
{
    DL_UART_Extend_clearInterruptStatus(HMI_INST,
        DL_UART_INTERRUPT_RX_TIMEOUT_ERROR);
}
