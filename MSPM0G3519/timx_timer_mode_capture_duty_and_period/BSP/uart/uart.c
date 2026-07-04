/**
 * @file    uart.c
 * @brief   UART 串口通信（基于 DL_UART 直接驱动）
 *
 *   使用 UART0 (PA10=TX, PA11=RX) 提供控制台串口功能。
 */

#include "uart.h"
#include "App/app_config.h"
#include "ti_msp_dl_config.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool s_uart_inited = false;

void Serial_Init(void)
{
    if (!s_uart_inited) {
        DL_UART_enablePower(UART0);

        /* PA10 = UART0 TX, PA11 = UART0 RX */
        DL_GPIO_initPeripheralOutputFunction(
            IOMUX_PINCM21, IOMUX_PINCM21_PF_UART0_TX);
        DL_GPIO_initPeripheralInputFunction(
            IOMUX_PINCM22, IOMUX_PINCM22_PF_UART0_RX);

        DL_UART_configBaudRate(UART0, CPUCLK_FREQ, CONSOLE_BAUDRATE);
        DL_UART_enable(UART0);

        s_uart_inited = true;
    }
}

void Serial_Send(const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        DL_UART_transmitDataBlocking(UART0, data[i]);
    }
}

void Serial_Printf(const char *format, ...)
{
    char tmp[128] = {0};
    va_list argptr;
    int len;

    va_start(argptr, format);
    len = vsnprintf(tmp, sizeof(tmp) - 1, format, argptr);
    va_end(argptr);

    if (len <= 0) return;
    if (len >= (int)sizeof(tmp)) len = (int)sizeof(tmp) - 1;

    Serial_Send((const uint8_t *)tmp, (uint16_t)len);
}
