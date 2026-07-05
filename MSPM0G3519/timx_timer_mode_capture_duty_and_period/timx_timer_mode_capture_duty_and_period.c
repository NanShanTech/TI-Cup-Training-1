#include "ti_msp_dl_config.h"

#include "App/app.h"
#include "BSP/bsp.h"
#include "My_Task/tasks.h"

uint16_t g_adc_buffer[FFT_N];  /* ADC DMA 采样缓冲区 — 2048 点 */

int main(void)
{
    /* ---- 板级 + 串口初始化 ---- */
    BSP_Init();
    Serial_Init();
    Serial_RxInit();          /* 开启 RX 空闲中断 */

    /* ---- ADC 采样链路：定时器触发 → DMA 搬运 → 中断通知 ---- */
    SampleTimer_Init();
    SampleTimer_SetRate(BSP_ADC_DEFAULT_SAMPLE_RATE);
    ADC_Init();
    ADC_StartNormal(g_adc_buffer, FFT_N);

    /* ---- 时间触发调度器 ---- */
    Scheduler_Init();
    Scheduler_RegTask_10ms(ADC_Proc);
    Scheduler_RegTask_1sec(UART_Proc);

    while (1) {
        Scheduler_Run();
    }
}

/* ---- 1ms 系统时基（SysTick 中断）---- */
void SysTick_Handler(void)
{
    g_ms_tick++;
}

/* ---- ADC DMA 完成中断 ---- */
void ADC12_0_INST_IRQHandler(void)
{
    ADC_IRQHandler();
}

/* ---- UART 空闲中断---- */
void HMI_INST_IRQHandler(void)
{
    if (DL_UART_Extend_getPendingInterrupt(HMI_INST)
        == DL_UART_IIDX_RX_TIMEOUT_ERROR) {
        uint8_t byte;
        while (Serial_RxFifoReadByte(&byte)) {
            Serial_RxPushByte(byte);
        }
    }
    Serial_RxClearFlags();
}
