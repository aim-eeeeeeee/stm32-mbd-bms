#include "bsp_watchdog.h"

#include "stm32f1xx.h"

#define BSP_WATCHDOG_START_KEY            (0xCCCCU)
#define BSP_WATCHDOG_WRITE_ACCESS_KEY     (0x5555U)
#define BSP_WATCHDOG_REFRESH_KEY          (0xAAAAU)
#define BSP_WATCHDOG_PRESCALER_DIV_32     (3U)
#define BSP_WATCHDOG_RELOAD_VALUE         (2499U)
#define BSP_WATCHDOG_UPDATE_WAIT_LIMIT    (100000U)

bsp_status_t bsp_watchdog_init(void)
{
    uint32_t wait_count = BSP_WATCHDOG_UPDATE_WAIT_LIMIT;

    /* 40 kHz nominal LSI / 32 / (2499 + 1) gives about a 2 s timeout. */
    IWDG->KR = BSP_WATCHDOG_START_KEY;
    IWDG->KR = BSP_WATCHDOG_WRITE_ACCESS_KEY;
    IWDG->PR = BSP_WATCHDOG_PRESCALER_DIV_32;
    IWDG->RLR = BSP_WATCHDOG_RELOAD_VALUE;

    while (((IWDG->SR & (IWDG_SR_PVU | IWDG_SR_RVU)) != 0U) &&
           (wait_count > 0U))
    {
        --wait_count;
    }

    if (wait_count == 0U)
    {
        return BSP_STATUS_GENERIC_HW_ERR;
    }

    bsp_watchdog_refresh();
    return BSP_STATUS_SUCCESS;
}

void bsp_watchdog_refresh(void)
{
    IWDG->KR = BSP_WATCHDOG_REFRESH_KEY;
}
