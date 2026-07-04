#include "delay.h"
#include "ti_msp_dl_config.h"

/* SysTick 重载值 = 计数值范围 + 1（Cortex-M0+ SysTick 为 24 位递减计数器） */
#define DELAY_RELOAD (SysTick->LOAD + 1U)

void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;

    ticks = nus * (CPUCLK_FREQ / 1000000U); /* 80 tick/us @ 80 MHz */
    told  = SysTick->VAL;

    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += DELAY_RELOAD - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks) break;
        }
    }
}

void delay_ms(uint32_t nms)
{
    uint32_t i;
    for (i = 0; i < nms; i++) {
        delay_us(1000);
    }
}
