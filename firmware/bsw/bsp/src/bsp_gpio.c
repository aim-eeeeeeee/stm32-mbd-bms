#include "bsp_gpio.h"
#include "bsp_time.h"
#include "main.h"

#define BSP_BQ_WAKE_LOW_SETUP_MS (1U)
#define BSP_BQ_WAKE_HIGH_MS      (5U)
#define BSP_BQ_STARTUP_WAIT_MS   (10U)
#define BSP_SYSTEM_POWER_WAIT_MS (10U)

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

void bsp_gpio_system_power_enable(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(SYS_POWER_ENABLE_GPIO_Port,
                      SYS_POWER_ENABLE_Pin,
                      GPIO_PIN_SET);
    gpio_init.Pin = SYS_POWER_ENABLE_Pin;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SYS_POWER_ENABLE_GPIO_Port, &gpio_init);

    bsp_time_delay_ms(BSP_SYSTEM_POWER_WAIT_MS);
}

void bsp_gpio_bq_wake_pulse(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(BQ_WAKE_GPIO_Port, BQ_WAKE_Pin, GPIO_PIN_RESET);
    gpio_init.Pin = BQ_WAKE_Pin;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BQ_WAKE_GPIO_Port, &gpio_init);

    bsp_time_delay_ms(BSP_BQ_WAKE_LOW_SETUP_MS);
    HAL_GPIO_WritePin(BQ_WAKE_GPIO_Port, BQ_WAKE_Pin, GPIO_PIN_SET);
    bsp_time_delay_ms(BSP_BQ_WAKE_HIGH_MS);
    HAL_GPIO_WritePin(BQ_WAKE_GPIO_Port, BQ_WAKE_Pin, GPIO_PIN_RESET);
    bsp_time_delay_ms(BSP_BQ_STARTUP_WAIT_MS);
}
