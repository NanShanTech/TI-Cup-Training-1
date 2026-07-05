#include "ti_msp_dl_config.h"

#include "App/app.h"
#include "BSP/bsp.h"
#include "My_Task/tasks.h"

uint16_t g_adc_buffer[FFT_Q15_N];  /* ADC DMA 采样缓冲区 */

int main(void)
{
    /* ---- 板级 + 串口初始化 ---- */
    BSP_Init();
    Serial_Init();
    HMI_Serial_Init();

    /* ---- ADC 采样链路：定时器触发 → DMA 搬运 → 中断通知 ---- */
    SampleTimer_Init();
    SampleTimer_SetRate(BSP_ADC_DEFAULT_SAMPLE_RATE);
    ADC_Init();
    ADC_StartNormal(g_adc_buffer, FFT_Q15_N);

    /* ---- 时间触发调度器 ---- */
    Scheduler_Init();
    Scheduler_RegTask_10ms(ADC_Proc);    /* 每 10ms 处理一次 ADC 数据 */
    Scheduler_RegTask_1sec(UART_Proc);   /* 每秒上报一次结果           */

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
