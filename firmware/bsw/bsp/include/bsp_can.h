#ifndef BSP_CAN_H
#define BSP_CAN_H

#include <stdbool.h>
#include <stdint.h>

#include "bsp_status.h"

/** Maximum number of payload bytes in a classic CAN frame. */
#define BSP_CAN_MAX_DATA_LENGTH (8U)

/** Identifier format owned by the project rather than STM32 HAL. */
typedef enum {
    BSP_CAN_ID_STANDARD = 0,
    BSP_CAN_ID_EXTENDED,
} bsp_can_id_type_t;

/** Frame format owned by the project rather than STM32 HAL. */
typedef enum {
    BSP_CAN_FRAME_DATA = 0,
    BSP_CAN_FRAME_REMOTE,
} bsp_can_frame_type_t;

/** HAL-independent representation of one classic CAN frame. */
typedef struct {
    uint32_t identifier;
    bsp_can_id_type_t id_type;
    bsp_can_frame_type_t frame_type;
    uint8_t data_length;
    uint8_t data[BSP_CAN_MAX_DATA_LENGTH];
} bsp_can_frame_t;

/**
 * @brief Configure the diagnostic accept-all filter and keep the transceiver off.
 * @note MX_CAN_Init() and MX_GPIO_Init() must run before this function.
 */
bsp_status_t bsp_can_init(void);

/**
 * @brief Logically control active-low CAN transceiver power.
 * @param enabled true powers the transceiver; false powers it off.
 * @return BSP_STATUS_NOT_INIT if power-on is requested before BSP CAN init.
 * @note Power-on is allowed only after a properly terminated test bus is connected.
 */
bsp_status_t bsp_can_transceiver_set_power(bool enabled);

/**
 * @brief Start the MCU CAN controller after the transceiver is powered.
 * @note Normal-mode start requires CAN_RX to observe a recessive bus level.
 */
bsp_status_t bsp_can_start(void);

/** Stop the MCU CAN controller after first powering the transceiver off. */
bsp_status_t bsp_can_stop(void);

/**
 * @brief Queue one frame for transmission.
 * @param frame Project-owned frame to map into an STM32 HAL transmit header.
 * @param timeout_ms Maximum wait for a free mailbox; zero means non-blocking.
 * @note Success means queued, not necessarily acknowledged on the physical bus.
 */
bsp_status_t bsp_can_transmit(const bsp_can_frame_t *frame,
                              uint32_t timeout_ms);

/** Return true when at least one frame is waiting in receive FIFO0. */
bool bsp_can_receive_available(void);

/** Read one frame from FIFO0, or return BSP_STATUS_BUSY if FIFO0 is empty. */
bsp_status_t bsp_can_receive(bsp_can_frame_t *frame);

#endif /* BSP_CAN_H */
