#include "uart.h"

#include "BSP/bsp.h"
#include "ti_msp_dl_config.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

static bool s_xds_uart_inited = false;
static bool s_hmi_uart_inited  = false;

/* ---- 内部辅助 ---- */

static void UART_SendBlocking(UART_Regs *uart, const uint8_t *data, uint16_t len)
{
    if (data == NULL) {
        return;
    }
    for (uint16_t i = 0; i < len; i++) {
        DL_UART_transmitDataBlocking(uart, data[i]);
    }
}

static void UART_PrintfTo(UART_Regs *uart, const char *format, va_list args)
{
    char tmp[128];
    int  len;

    len = vsnprintf(tmp, sizeof(tmp), format, args);
    if (len <= 0) {
        return;
    }
    if (len >= (int)sizeof(tmp)) {
        len = (int)sizeof(tmp) - 1;
    }
    UART_SendBlocking(uart, (const uint8_t *)tmp, (uint16_t)len);
}

/* ---- XDS 调试串口 ---- */

void Serial_Init(void)
{
    if (s_xds_uart_inited) {
        return;
    }
    DL_UART_Main_enablePower(XDS_INST);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_XDS_IOMUX_TX, GPIO_XDS_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_XDS_IOMUX_RX, GPIO_XDS_IOMUX_RX_FUNC);
    DL_UART_configBaudRate(XDS_INST, CPUCLK_FREQ / 2U, BSP_CONSOLE_BAUDRATE);
    DL_UART_enable(XDS_INST);

    s_xds_uart_inited = true;
}

void Serial_Send(const uint8_t *data, uint16_t len)
{
    if (!s_xds_uart_inited) {
        Serial_Init();
    }
    UART_SendBlocking(XDS_INST, data, len);
}

void Serial_Printf(const char *format, ...)
{
    va_list argptr;

    if (!s_xds_uart_inited) {
        Serial_Init();
    }
    va_start(argptr, format);
    UART_PrintfTo(XDS_INST, format, argptr);
    va_end(argptr);
}

/* ---- HMI 触摸屏串口 ---- */

void HMI_Serial_Init(void)
{
    if (s_hmi_uart_inited) {
        return;
    }
    DL_UART_Main_enablePower(HMI_INST);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_HMI_IOMUX_TX, GPIO_HMI_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_HMI_IOMUX_RX, GPIO_HMI_IOMUX_RX_FUNC);
    DL_UART_configBaudRate(HMI_INST, CPUCLK_FREQ, HMI_BAUD_RATE);
    DL_UART_enable(HMI_INST);

    s_hmi_uart_inited = true;
}

void HMI_Serial_Send(const uint8_t *data, uint16_t len)
{
    if (!s_hmi_uart_inited) {
        HMI_Serial_Init();
    }
    UART_SendBlocking(HMI_INST, data, len);
}

void HMI_Serial_Printf(const char *format, ...)
{
    va_list argptr;

    if (!s_hmi_uart_inited) {
        HMI_Serial_Init();
    }
    va_start(argptr, format);
    UART_PrintfTo(HMI_INST, format, argptr);
    va_end(argptr);
}
