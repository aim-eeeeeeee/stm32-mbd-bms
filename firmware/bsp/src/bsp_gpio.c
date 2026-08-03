#include "bsp_gpio.h"
#include "main.h"

void bsp_gpio_status_led_set(bool enabled)
{
    HAL_GPIO_WritePin(
        STATUS_LED_GPIO_Port,
        STATUS_LED_Pin,
        enabled ? GPIO_PIN_RESET : GPIO_PIN_SET
    );
}

void bsp_gpio_status_led_toggle(void)
{
    HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
}
