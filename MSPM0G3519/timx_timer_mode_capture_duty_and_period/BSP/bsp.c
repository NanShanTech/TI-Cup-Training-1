#include "BSP/bsp.h"

#include "ti_msp_dl_config.h"
#include <ti/driverlib/m0p/dl_core.h>

#define BSP_RESET_VECTKEY          (0x5FAUL << 16U)
#define SAMPLE_TIMER_CLOCK_HZ      (40000000UL)  /* BUSCLK=80MHz, /2=40MHz */

static uint32_t s_sample_timer_rate_hz = BSP_ADC_DEFAULT_SAMPLE_RATE;

void BSP_Init(void)
{
    SYSCFG_DL_init();
}

uint32_t BSP_GetSystemClock(void)
{
    return CPUCLK_FREQ;
}

void BSP_Reset(void)
{
    __disable_irq();
    SCB->AIRCR = (BSP_RESET_VECTKEY | SCB_AIRCR_SYSRESETREQ_Msk);
    __DSB();
    while (1) {
        __WFI();
    }
}

uint32_t BSP_GetDeviceId(void)
{
    return DL_SYSCTL_getDeviceID();
}

/* ---- 延时（基于 delay_cycles 指令循环）---- */

void BSP_DelayUs(uint32_t us)
{
    delay_cycles(us * (CPUCLK_FREQ / 1000000UL));
}

void BSP_DelayMs(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++) {
        delay_cycles(CPUCLK_FREQ / 1000UL);
    }
}

/* ---- 采样定时器（TIMER_0 触发 ADC）---- */

bool SampleTimer_Init(void)
{
    DL_TimerA_setCounterValueAfterEnable(
        TIMER_0_INST, DL_TIMER_COUNT_AFTER_EN_LOAD_VAL);
    SampleTimer_Stop();
    return SampleTimer_SetRate(BSP_ADC_DEFAULT_SAMPLE_RATE);
}

bool SampleTimer_SetRate(uint32_t sampleRateHz)
{
    uint32_t loadValue;

    if (sampleRateHz == 0U) {
        return false;
    }

    loadValue = (SAMPLE_TIMER_CLOCK_HZ + (sampleRateHz / 2U)) / sampleRateHz;
    if (loadValue == 0U) {
        return false;
    }

    loadValue -= 1U;

    SampleTimer_Stop();
    DL_TimerA_setLoadValue(TIMER_0_INST, loadValue);
    DL_TimerA_setTimerCount(TIMER_0_INST, loadValue);
    s_sample_timer_rate_hz = sampleRateHz;

    return true;
}

uint32_t SampleTimer_GetRate(void)
{
    return s_sample_timer_rate_hz;
}

void SampleTimer_Start(void)
{
    uint32_t loadValue = DL_TimerA_getLoadValue(TIMER_0_INST);

    DL_TimerA_setTimerCount(TIMER_0_INST, loadValue);
    DL_TimerA_startCounter(TIMER_0_INST);
}

void SampleTimer_Stop(void)
{
    DL_TimerA_stopCounter(TIMER_0_INST);
}
