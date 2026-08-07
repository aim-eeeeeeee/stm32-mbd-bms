#ifndef BSP_WATCHDOG_H
#define BSP_WATCHDOG_H

#include "bsp_status.h"

/**
 * @brief Start the independent watchdog with an approximately 2 s timeout.
 *
 * Once started, the STM32 independent watchdog cannot be stopped except by a
 * device reset.
 *
 * @return BSP_STATUS_SUCCESS when configured, otherwise a hardware error.
 */
bsp_status_t bsp_watchdog_init(void);

/**
 * @brief Reload the independent watchdog countdown.
 */
void bsp_watchdog_refresh(void);

#endif /* BSP_WATCHDOG_H */
