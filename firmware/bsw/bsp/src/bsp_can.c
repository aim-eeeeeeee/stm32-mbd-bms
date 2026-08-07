#include "bsp_can.h"

#include "bsp_time.h"
#include "can.h"
#include "main.h"

#include <stddef.h>

#define BSP_CAN_STANDARD_ID_MAX (0x7FFUL)
#define BSP_CAN_EXTENDED_ID_MAX (0x1FFFFFFFUL)

/* These flags describe the BSP lifecycle; they are not CAN protocol state. */
static bool s_filter_configured;
static bool s_can_started;
static bool s_transceiver_powered;

static bsp_status_t bsp_can_translate_hal_status(HAL_StatusTypeDef hal_status)
{
    uint32_t error_code;
    const uint32_t communication_errors =
        HAL_CAN_ERROR_EWG | HAL_CAN_ERROR_EPV | HAL_CAN_ERROR_BOF |
        HAL_CAN_ERROR_STF | HAL_CAN_ERROR_FOR | HAL_CAN_ERROR_ACK |
        HAL_CAN_ERROR_BR | HAL_CAN_ERROR_BD | HAL_CAN_ERROR_CRC |
        HAL_CAN_ERROR_RX_FOV0 | HAL_CAN_ERROR_RX_FOV1 |
        HAL_CAN_ERROR_TX_ALST0 | HAL_CAN_ERROR_TX_TERR0 |
        HAL_CAN_ERROR_TX_ALST1 | HAL_CAN_ERROR_TX_TERR1 |
        HAL_CAN_ERROR_TX_ALST2 | HAL_CAN_ERROR_TX_TERR2;

    switch (hal_status)
    {
        case HAL_OK:
            return BSP_STATUS_SUCCESS;

        case HAL_BUSY:
            return BSP_STATUS_BUSY;

        case HAL_TIMEOUT:
            return BSP_STATUS_TIMEOUT;

        case HAL_ERROR:
            error_code = HAL_CAN_GetError(&hcan);

            if ((error_code & HAL_CAN_ERROR_TIMEOUT) != 0U)
            {
                return BSP_STATUS_TIMEOUT;
            }

            if ((error_code & HAL_CAN_ERROR_PARAM) != 0U)
            {
                return BSP_STATUS_INVALID_ARG;
            }

            if ((error_code & (HAL_CAN_ERROR_NOT_INITIALIZED |
                               HAL_CAN_ERROR_NOT_READY |
                               HAL_CAN_ERROR_NOT_STARTED)) != 0U)
            {
                return BSP_STATUS_NOT_INIT;
            }

            if ((error_code & communication_errors) != 0U)
            {
                return BSP_STATUS_COMM_ERR;
            }

            return BSP_STATUS_GENERIC_HW_ERR;

        default:
            return BSP_STATUS_GENERIC_HW_ERR;
    }
}

static bsp_status_t bsp_can_validate_frame(const bsp_can_frame_t *frame)
{
    if (frame == NULL)
    {
        return BSP_STATUS_INVALID_ARG;
    }

    if (frame->data_length > BSP_CAN_MAX_DATA_LENGTH)
    {
        return BSP_STATUS_INVALID_ARG;
    }

    if ((frame->id_type != BSP_CAN_ID_STANDARD) &&
        (frame->id_type != BSP_CAN_ID_EXTENDED))
    {
        return BSP_STATUS_INVALID_ARG;
    }

    if ((frame->frame_type != BSP_CAN_FRAME_DATA) &&
        (frame->frame_type != BSP_CAN_FRAME_REMOTE))
    {
        return BSP_STATUS_INVALID_ARG;
    }

    if (((frame->id_type == BSP_CAN_ID_STANDARD) &&
         (frame->identifier > BSP_CAN_STANDARD_ID_MAX)) ||
        ((frame->id_type == BSP_CAN_ID_EXTENDED) &&
         (frame->identifier > BSP_CAN_EXTENDED_ID_MAX)))
    {
        return BSP_STATUS_INVALID_ARG;
    }

    return BSP_STATUS_SUCCESS;
}

bsp_status_t bsp_can_transceiver_set_power(bool enabled)
{
    /* PA15 is active-low: RESET powers the transceiver, SET powers it off. */
    if (enabled && !s_filter_configured)
    {
        return BSP_STATUS_NOT_INIT;
    }

    HAL_GPIO_WritePin(CAN_POWER_N_GPIO_Port,
                      CAN_POWER_N_Pin,
                      enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
    s_transceiver_powered = enabled;
    return BSP_STATUS_SUCCESS;
}

bsp_status_t bsp_can_init(void)
{
    CAN_FilterTypeDef filter = {0};
    HAL_StatusTypeDef hal_status;

    /* Establish the safe hardware state before configuring the controller. */
    (void)bsp_can_transceiver_set_power(false);
    s_can_started = false;
    s_transceiver_powered = false;
    s_filter_configured = false;

    /*
     * Diagnostic bring-up filter: ID=0 and mask=0 accept every standard,
     * extended, data, and remote frame into FIFO0. Production firmware must
     * replace this broad policy with protocol-specific acceptance filters.
     */
    filter.FilterIdHigh = 0U;
    filter.FilterIdLow = 0U;
    filter.FilterMaskIdHigh = 0U;
    filter.FilterMaskIdLow = 0U;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterBank = 0U;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterActivation = ENABLE;
    filter.SlaveStartFilterBank = 14U; /* Ignored on this single-CAN MCU. */

    hal_status = HAL_CAN_ConfigFilter(&hcan, &filter);
    if (hal_status == HAL_OK)
    {
        s_filter_configured = true;
    }

    return bsp_can_translate_hal_status(hal_status);
}

bsp_status_t bsp_can_start(void)
{
    HAL_StatusTypeDef hal_status;

    if (!s_filter_configured)
    {
        return BSP_STATUS_NOT_INIT;
    }

    /* In normal mode, CAN_RX must see a recessive level before start can finish. */
    if (!s_transceiver_powered)
    {
        return BSP_STATUS_NOT_INIT;
    }

    if (s_can_started)
    {
        return BSP_STATUS_SUCCESS;
    }

    hal_status = HAL_CAN_Start(&hcan);
    if (hal_status == HAL_OK)
    {
        s_can_started = true;
    }

    return bsp_can_translate_hal_status(hal_status);
}

bsp_status_t bsp_can_stop(void)
{
    HAL_StatusTypeDef hal_status;

    /* Power off first so even a failed controller stop cannot drive the bus. */
    (void)bsp_can_transceiver_set_power(false);

    if (!s_filter_configured)
    {
        return BSP_STATUS_NOT_INIT;
    }

    if (!s_can_started)
    {
        return BSP_STATUS_SUCCESS;
    }

    hal_status = HAL_CAN_Stop(&hcan);
    if (hal_status == HAL_OK)
    {
        s_can_started = false;
    }

    return bsp_can_translate_hal_status(hal_status);
}

bsp_status_t bsp_can_transmit(const bsp_can_frame_t *frame,
                              uint32_t timeout_ms)
{
    CAN_TxHeaderTypeDef header = {0};
    uint8_t payload[BSP_CAN_MAX_DATA_LENGTH] = {0};
    uint32_t mailbox;
    uint32_t start_ms;
    uint8_t index;
    bsp_status_t validation_status = bsp_can_validate_frame(frame);

    if (validation_status != BSP_STATUS_SUCCESS)
    {
        return validation_status;
    }

    if (!s_can_started)
    {
        return BSP_STATUS_NOT_INIT;
    }

    /* Map the project frame into the HAL-only representation at this boundary. */
    header.IDE = (frame->id_type == BSP_CAN_ID_EXTENDED) ? CAN_ID_EXT : CAN_ID_STD;
    header.RTR = (frame->frame_type == BSP_CAN_FRAME_REMOTE) ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    header.StdId = (frame->id_type == BSP_CAN_ID_STANDARD) ? frame->identifier : 0U;
    header.ExtId = (frame->id_type == BSP_CAN_ID_EXTENDED) ? frame->identifier : 0U;
    header.DLC = frame->data_length;
    header.TransmitGlobalTime = DISABLE;

    for (index = 0U; index < BSP_CAN_MAX_DATA_LENGTH; ++index)
    {
        payload[index] = frame->data[index];
    }

    /* HAL does not wait for a mailbox, so the BSP supplies bounded waiting. */
    start_ms = bsp_time_now_ms();
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0U)
    {
        if (timeout_ms == 0U)
        {
            return BSP_STATUS_BUSY;
        }

        if ((bsp_time_now_ms() - start_ms) >= timeout_ms)
        {
            return BSP_STATUS_TIMEOUT;
        }
    }

    return bsp_can_translate_hal_status(
        HAL_CAN_AddTxMessage(&hcan, &header, payload, &mailbox));
}

bool bsp_can_receive_available(void)
{
    if (!s_can_started)
    {
        return false;
    }

    return HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) > 0U;
}

bsp_status_t bsp_can_receive(bsp_can_frame_t *frame)
{
    CAN_RxHeaderTypeDef header = {0};
    uint8_t payload[BSP_CAN_MAX_DATA_LENGTH] = {0};
    uint8_t index;
    HAL_StatusTypeDef hal_status;

    if (frame == NULL)
    {
        return BSP_STATUS_INVALID_ARG;
    }

    if (!s_can_started)
    {
        return BSP_STATUS_NOT_INIT;
    }

    if (HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) == 0U)
    {
        return BSP_STATUS_BUSY;
    }

    hal_status = HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &header, payload);
    if (hal_status != HAL_OK)
    {
        return bsp_can_translate_hal_status(hal_status);
    }

    if (((header.IDE != CAN_ID_STD) && (header.IDE != CAN_ID_EXT)) ||
        ((header.RTR != CAN_RTR_DATA) && (header.RTR != CAN_RTR_REMOTE)) ||
        (header.DLC > BSP_CAN_MAX_DATA_LENGTH))
    {
        return BSP_STATUS_GENERIC_HW_ERR;
    }

    frame->id_type = (header.IDE == CAN_ID_EXT) ? BSP_CAN_ID_EXTENDED : BSP_CAN_ID_STANDARD;
    frame->frame_type = (header.RTR == CAN_RTR_REMOTE) ? BSP_CAN_FRAME_REMOTE : BSP_CAN_FRAME_DATA;
    frame->identifier = (header.IDE == CAN_ID_EXT) ? header.ExtId : header.StdId;
    frame->data_length = (uint8_t)header.DLC;

    for (index = 0U; index < BSP_CAN_MAX_DATA_LENGTH; ++index)
    {
        /* Bytes beyond DLC are zero because payload was zero-initialized. */
        frame->data[index] = payload[index];
    }

    return BSP_STATUS_SUCCESS;
}
