#include "bq76940.h"

#include <stddef.h>

#include "bsp_i2c.h"

#define BQ76940_CRC_POLYNOMIAL (0x07U)
#define BQ76940_CRC_INITIAL    (0x00U)
#define BQ76940_I2C_ADDRESS_7BIT (0x08U)
#define BQ76940_I2C_READ_ADDRESS_BYTE \
    ((uint8_t)((BQ76940_I2C_ADDRESS_7BIT << 1U) | 0x01U))
#define BQ76940_I2C_TIMEOUT_MS        (50U)
#define BQ76940_MAX_LOGICAL_BYTES     (30U)
#define BQ76940_WIRE_BYTES_PER_DATA   (2U)
#define BQ76940_MAX_WIRE_BYTES \
    (BQ76940_MAX_LOGICAL_BYTES * BQ76940_WIRE_BYTES_PER_DATA)

static uint8_t bq76940_crc8(const uint8_t *data, uint16_t length)
{
    uint8_t crc = BQ76940_CRC_INITIAL;

    if (data == NULL)
    {
        return crc;
    }

    for (uint16_t byte_index = 0U; byte_index < length; ++byte_index)
    {
        crc ^= data[byte_index];

        for (uint8_t bit_index = 0U; bit_index < 8U; ++bit_index)
        {
            if ((crc & 0x80U) != 0U)
            {
                crc = (uint8_t)((crc << 1U) ^ BQ76940_CRC_POLYNOMIAL);
            }
            else
            {
                crc = (uint8_t)(crc << 1U);
            }
        }
    }

    return crc;
}

static bq76940_status_t bq76940_translate_bsp_status(bsp_status_t bsp_status)
{
    switch (bsp_status)
    {
        case BSP_STATUS_SUCCESS:
            return BQ76940_STATUS_OK;

        case BSP_STATUS_INVALID_ARG:
            return BQ76940_STATUS_INVALID_ARG;

        case BSP_STATUS_NOT_INIT:
            return BQ76940_STATUS_NOT_INIT;

        case BSP_STATUS_TIMEOUT:
            return BQ76940_STATUS_TIMEOUT;

        case BSP_STATUS_BUSY:
        case BSP_STATUS_COMM_ERR:
        case BSP_STATUS_GENERIC_HW_ERR:
        default:
            return BQ76940_STATUS_TRANSPORT_ERR;
    }
}

static bq76940_status_t bq76940_read_registers_crc(uint8_t first_register,
                                                   uint8_t *data,
                                                   uint8_t data_length)
{
    uint8_t wire_data[BQ76940_MAX_WIRE_BYTES];
    uint16_t wire_length;
    bsp_status_t bsp_status;
    bq76940_status_t driver_status;

    if ((data == NULL) || (data_length == 0U) ||
        (data_length > BQ76940_MAX_LOGICAL_BYTES))
    {
        return BQ76940_STATUS_INVALID_ARG;
    }

    wire_length = (uint16_t)data_length * BQ76940_WIRE_BYTES_PER_DATA;
    bsp_status = bsp_i2c_read(BQ76940_I2C_ADDRESS_7BIT,
                              first_register,
                              wire_data,
                              wire_length,
                              BQ76940_I2C_TIMEOUT_MS);
    driver_status = bq76940_translate_bsp_status(bsp_status);

    if (driver_status != BQ76940_STATUS_OK)
    {
        return driver_status;
    }

    for (uint8_t data_index = 0U; data_index < data_length; ++data_index)
    {
        uint16_t wire_index =
            (uint16_t)data_index * BQ76940_WIRE_BYTES_PER_DATA;
        uint8_t received_crc = wire_data[wire_index + 1U];
        uint8_t calculated_crc;

        if (data_index == 0U)
        {
            uint8_t first_crc_input[2] = {
                BQ76940_I2C_READ_ADDRESS_BYTE,
                wire_data[wire_index],
            };

            calculated_crc = bq76940_crc8(first_crc_input,
                                           (uint16_t)sizeof(first_crc_input));
        }
        else
        {
            calculated_crc = bq76940_crc8(&wire_data[wire_index], 1U);
        }

        if (calculated_crc != received_crc)
        {
            return BQ76940_STATUS_CRC_ERR;
        }
    }

    for (uint8_t data_index = 0U; data_index < data_length; ++data_index)
    {
        uint16_t wire_index =
            (uint16_t)data_index * BQ76940_WIRE_BYTES_PER_DATA;
        data[data_index] = wire_data[wire_index];
    }

    return BQ76940_STATUS_OK;
}
