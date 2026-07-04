/**
 * @file    timx_timer_mode_capture_duty_and_period.c
 * @brief   Main entry for the MSPM0G3519 timer capture project.
 *
 * Keep main as a module assembly point:
 *   1. Initialize hardware and application modules.
 *   2. Register scheduler tasks.
 *   3. Start capture.
 *   4. Run only Scheduler_Run() in the main loop.
 */

#include "ti_msp_dl_config.h"
#include "BSP/bsp.h"
#include "App/app.h"
#include "My_Task/tasks.h"

/* ---- ADC DMA 采样完成标志（由 DMA ISR 置位）---- */
volatile uint8_t g_adc_dma_done;

/* ---- ADC DMA 采样缓冲区（与 SysConfig DMA 传输大小一致）---- */
uint16_t g_adc_buffer[FFT_Q15_N];

int main(void)
{
    BSP_Init();
    Serial_Init();

    Scheduler_Init();
    Scheduler_RegTask_10ms(ADC_Proc);
    Scheduler_RegTask_1sec(UART_Proc);

    while (1) {
        Scheduler_Run();
        
    }
}

void SysTick_Handler(void)
{
    g_ms_tick++;
    DL_GPIO_togglePins(LED_PORT, LED_PIN_0_PIN);
    
}

void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        
        case DL_ADC12_IIDX_DMA_DONE:
            g_adc_dma_done = 1;
            break;
        default:
            break;
    }
}