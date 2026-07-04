#include "BSP/bsp.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/m0p/dl_core.h>

#define BSP_RESET_VECTKEY  (0x5FAUL << 16U)

/* ================================================================
 * 系统 API
 * ================================================================ */

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

/* ================================================================
 * 延时 API
 * ================================================================ */

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
