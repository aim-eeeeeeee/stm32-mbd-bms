#ifndef BSP_TIME_H
#define BSP_TIME_H

#include <stdint.h>

uint32_t bsp_time_now_ms(void);
void bsp_time_delay_ms(uint32_t delay_ms);

#endif /* BSP_TIME_H */
