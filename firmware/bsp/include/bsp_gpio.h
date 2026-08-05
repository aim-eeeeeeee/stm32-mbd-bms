#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include <stdbool.h>

void bsp_gpio_status_led_set(bool enabled);
void bsp_gpio_status_led_toggle(void);

/**
 * @brief Assert the board's active-high PB5 system-power control.
 */
void bsp_gpio_system_power_enable(void);

/**
 * @brief Apply the board-specific PB3 pulse that boots the BQ76940 via TS1.
 *
 * The output returns low before this function returns. This function does not
 * write any BQ76940 register or enable charge/discharge outputs.
 */
void bsp_gpio_bq_wake_pulse(void);

#endif /* BSP_GPIO_H */
