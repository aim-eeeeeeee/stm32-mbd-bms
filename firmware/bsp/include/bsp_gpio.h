#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include <stdbool.h>

void bsp_gpio_status_led_set(bool enabled);
void bsp_gpio_status_led_toggle(void);

#endif /* BSP_GPIO_H */