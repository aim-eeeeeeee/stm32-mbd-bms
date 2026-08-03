#include "bsp_i2c.h"
#include "i2c.h"
#include <stddef.h>

#define BSP_I2C_MAX_7BIT_ADDRESS (0x7FU) /* Limit public addresses to an unshifted 7-bit value. */

static bsp_status_t bsp_i2c_translate_hal_status(HAL_StatusTypeDef hal_status)
{
    uint32_t error_code;

    switch (hal_status)
    {
        case HAL_OK:
            return BSP_STATUS_SUCCESS;

        case HAL_BUSY:
            return BSP_STATUS_BUSY;

        case HAL_TIMEOUT:
            return BSP_STATUS_TIMEOUT;

        case HAL_ERROR:
            error_code = HAL_I2C_GetError(&hi2c1);
            if ((error_code & HAL_I2C_ERROR_TIMEOUT) != 0U)
            {
                return BSP_STATUS_TIMEOUT;
            }
            if ((error_code & (HAL_I2C_ERROR_BERR | HAL_I2C_ERROR_ARLO | HAL_I2C_ERROR_AF | HAL_I2C_ERROR_OVR)) != 0U)
            {
                return BSP_STATUS_COMM_ERR;
            }
            return BSP_STATUS_GENERIC_HW_ERR;

        default:
            return BSP_STATUS_GENERIC_HW_ERR;
    }
}

static bsp_status_t bsp_i2c_get_readiness(void)
{
    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(&hi2c1); /* Read the state established by MX_I2C1_Init(). */

    switch (state)
    {
        case HAL_I2C_STATE_RESET:
            return BSP_STATUS_NOT_INIT;

        case HAL_I2C_STATE_READY:
            return BSP_STATUS_SUCCESS;

        case HAL_I2C_STATE_TIMEOUT:
            return BSP_STATUS_TIMEOUT;

        case HAL_I2C_STATE_ERROR:
            return bsp_i2c_translate_hal_status(HAL_ERROR);

        default:
            return BSP_STATUS_BUSY;
    }
}

static bsp_status_t bsp_i2c_validate_transfer(uint8_t device_address_7bit, const void *data, uint16_t data_length, uint32_t timeout_ms)
{
    if ((device_address_7bit > BSP_I2C_MAX_7BIT_ADDRESS) || (data == NULL) || (data_length == 0U) || (timeout_ms == 0U))
    {
        return BSP_STATUS_INVALID_ARG;
    }

    return bsp_i2c_get_readiness(); /* Require the generated I2C1 peripheral to be initialized and idle. */
}

bsp_status_t bsp_i2c_init(void)
{
    return bsp_i2c_get_readiness();
}

bsp_status_t bsp_i2c_read_u8(uint8_t device_address_7bit, uint8_t register_address, uint8_t *value, uint32_t timeout_ms)
{
    return bsp_i2c_read(device_address_7bit, register_address, value, 1U, timeout_ms);
}

bsp_status_t bsp_i2c_write_u8(uint8_t device_address_7bit, uint8_t register_address, uint8_t value, uint32_t timeout_ms)
{
    return bsp_i2c_write(device_address_7bit, register_address, &value, 1U, timeout_ms);
}

bsp_status_t bsp_i2c_read(uint8_t device_address_7bit, uint8_t register_address, uint8_t *data, uint16_t data_length, uint32_t timeout_ms)
{
    bsp_status_t validation_status = bsp_i2c_validate_transfer(device_address_7bit, data, data_length, timeout_ms);
    HAL_StatusTypeDef hal_status;

    if (validation_status != BSP_STATUS_SUCCESS)
    {
        return validation_status;
    }

    hal_status = HAL_I2C_Mem_Read(&hi2c1, (uint16_t)((uint16_t)device_address_7bit << 1U), register_address, I2C_MEMADD_SIZE_8BIT, data, data_length, timeout_ms);
    return bsp_i2c_translate_hal_status(hal_status);
}

bsp_status_t bsp_i2c_write(uint8_t device_address_7bit, uint8_t register_address, const uint8_t *data, uint16_t data_length, uint32_t timeout_ms)
{
    bsp_status_t validation_status = bsp_i2c_validate_transfer(device_address_7bit, data, data_length, timeout_ms);
    HAL_StatusTypeDef hal_status;

    if (validation_status != BSP_STATUS_SUCCESS)
    {
        return validation_status;
    }

    hal_status = HAL_I2C_Mem_Write(&hi2c1, (uint16_t)((uint16_t)device_address_7bit << 1U), register_address, I2C_MEMADD_SIZE_8BIT, (uint8_t *)data, data_length, timeout_ms);
    return bsp_i2c_translate_hal_status(hal_status);
}
