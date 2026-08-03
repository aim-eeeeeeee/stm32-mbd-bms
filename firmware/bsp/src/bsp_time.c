#include "bsp_time.h"
#include "stm32f1xx_hal.h"

uint32_t bsp_time_now_ms(void) {
    return HAL_GetTick();
}

void bsp_time_delay_ms(uint32_t delay_ms) {
    HAL_Delay(delay_ms);
}
