#include "bq76940.h"

#include <stdbool.h>
#include <stdint.h>
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
#define BQ76940_REG_SYS_STAT       (0x00U)
#define BQ76940_REG_VC1_HI         (0x0CU)
#define BQ76940_REG_ADCGAIN1       (0x50U)
#define BQ76940_REG_ADCGAIN2       (0x59U)
#define BQ76940_CELL_REGISTER_BYTES \
    (BQ76940_CELL_COUNT * 2U)
#define BQ76940_ADCGAIN1_MASK      (0x0CU)
#define BQ76940_ADCGAIN1_SHIFT     (1U)
#define BQ76940_ADCGAIN2_MASK      (0xE0U)
#define BQ76940_ADCGAIN2_SHIFT     (5U)
#define BQ76940_ADC_GAIN_BASE_UV   (365)
#define BQ76940_ADC_GAIN_MIN_UV    (365)
#define BQ76940_ADC_GAIN_MAX_UV    (396)
#define BQ76940_CELL_HIGH_MASK      (0x3FU)
#define BQ76940_CELL_HIGH_SHIFT     (8U)
#define BQ76940_CELL_RAW_MAX        (0x3FFFU)
#define BQ76940_UV_PER_MV           (1000U)
#define BQ76940_UV_ROUND_TO_MV      (500U)

static bool bq76940_initialized;
static bq76940_calibration_t bq76940_calibration;

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

static bq76940_status_t bq76940_decode_calibration(
    uint8_t adc_gain1,
    uint8_t adc_offset,
    uint8_t adc_gain2,
    bq76940_calibration_t *calibration)
{
    uint8_t gain_code;
    int16_t gain_uv_per_lsb;

    if (calibration == NULL)
    {
        return BQ76940_STATUS_INVALID_ARG;
    }

    gain_code = (uint8_t)(((adc_gain1 & BQ76940_ADCGAIN1_MASK)
                           << BQ76940_ADCGAIN1_SHIFT) |
                          ((adc_gain2 & BQ76940_ADCGAIN2_MASK)
                           >> BQ76940_ADCGAIN2_SHIFT));
    gain_uv_per_lsb = (int16_t)(BQ76940_ADC_GAIN_BASE_UV + gain_code);

    if ((gain_uv_per_lsb < BQ76940_ADC_GAIN_MIN_UV) ||
        (gain_uv_per_lsb > BQ76940_ADC_GAIN_MAX_UV))
    {
        return BQ76940_STATUS_INVALID_DATA;
    }

    calibration->adc_gain_uv_per_lsb = gain_uv_per_lsb;
    calibration->adc_offset_mv = (int8_t)adc_offset;

    return BQ76940_STATUS_OK;
}

static uint16_t bq76940_decode_cell_raw(uint8_t high, uint8_t low)
{
    uint16_t high_bits = (uint16_t)(high & BQ76940_CELL_HIGH_MASK);

    return (uint16_t)((high_bits << BQ76940_CELL_HIGH_SHIFT) | low);
}

static bq76940_status_t bq76940_cell_raw_to_mv(
    uint16_t raw_count,
    const bq76940_calibration_t *calibration,
    uint16_t *cell_mv)
{
    uint32_t cell_uv;
    int32_t converted_mv;

    if ((calibration == NULL) || (cell_mv == NULL))
    {
        return BQ76940_STATUS_INVALID_ARG;
    }

    if ((raw_count > BQ76940_CELL_RAW_MAX) ||
        (calibration->adc_gain_uv_per_lsb < BQ76940_ADC_GAIN_MIN_UV) ||
        (calibration->adc_gain_uv_per_lsb > BQ76940_ADC_GAIN_MAX_UV))
    {
        return BQ76940_STATUS_INVALID_DATA;
    }

    cell_uv = (uint32_t)raw_count *
              (uint32_t)calibration->adc_gain_uv_per_lsb;
    converted_mv = (int32_t)((cell_uv + BQ76940_UV_ROUND_TO_MV) /
                             BQ76940_UV_PER_MV) +
                   (int32_t)calibration->adc_offset_mv;

    if ((converted_mv < 0) || (converted_mv > (int32_t)UINT16_MAX))
    {
        return BQ76940_STATUS_INVALID_DATA;
    }

    *cell_mv = (uint16_t)converted_mv;
    return BQ76940_STATUS_OK;
}

bq76940_status_t bq76940_init(void)
{
    uint8_t system_status;
    uint8_t gain1_and_offset[2];
    uint8_t adc_gain2;
    bq76940_calibration_t calibration;
    bq76940_status_t status;

    bq76940_initialized = false;

    status = bq76940_translate_bsp_status(bsp_i2c_init());
    if (status != BQ76940_STATUS_OK)
    {
        return status;
    }

    status = bq76940_read_registers_crc(BQ76940_REG_SYS_STAT,
                                        &system_status,
                                        1U);
    if (status != BQ76940_STATUS_OK)
    {
        return status;
    }

    status = bq76940_read_registers_crc(BQ76940_REG_ADCGAIN1,
                                        gain1_and_offset,
                                        (uint8_t)sizeof(gain1_and_offset));
    if (status != BQ76940_STATUS_OK)
    {
        return status;
    }

    status = bq76940_read_registers_crc(BQ76940_REG_ADCGAIN2,
                                        &adc_gain2,
                                        1U);
    if (status != BQ76940_STATUS_OK)
    {
        return status;
    }

    status = bq76940_decode_calibration(gain1_and_offset[0],
                                        gain1_and_offset[1],
                                        adc_gain2,
                                        &calibration);
    if (status != BQ76940_STATUS_OK)
    {
        return status;
    }

    bq76940_calibration = calibration;
    bq76940_initialized = true;

    return BQ76940_STATUS_OK;
}

bq76940_status_t bq76940_get_calibration(
    bq76940_calibration_t *calibration)
{
    if (calibration == NULL)
    {
        return BQ76940_STATUS_INVALID_ARG;
    }

    if (!bq76940_initialized)
    {
        return BQ76940_STATUS_NOT_INIT;
    }

    *calibration = bq76940_calibration;
    return BQ76940_STATUS_OK;
}

bq76940_status_t bq76940_read_status(uint8_t *system_status)
{
    if (system_status == NULL)
    {
        return BQ76940_STATUS_INVALID_ARG;
    }

    return bq76940_read_registers_crc(BQ76940_REG_SYS_STAT,
                                      system_status,
                                      1U);
}

bq76940_status_t bq76940_read_cell_voltages(
    uint16_t cell_mv[BQ76940_CELL_COUNT])
{
    uint8_t cell_registers[BQ76940_CELL_REGISTER_BYTES];
    uint16_t converted_cell_mv[BQ76940_CELL_COUNT];
    bq76940_status_t status;

    if (cell_mv == NULL)
    {
        return BQ76940_STATUS_INVALID_ARG;
    }

    if (!bq76940_initialized)
    {
        return BQ76940_STATUS_NOT_INIT;
    }

    status = bq76940_read_registers_crc(
        BQ76940_REG_VC1_HI,
        cell_registers,
        (uint8_t)sizeof(cell_registers));
    if (status != BQ76940_STATUS_OK)
    {
        return status;
    }

    for (uint8_t cell_index = 0U;
         cell_index < BQ76940_CELL_COUNT;
         ++cell_index)
    {
        uint8_t register_index = (uint8_t)(cell_index * 2U);
        uint16_t raw_count = bq76940_decode_cell_raw(
            cell_registers[register_index],
            cell_registers[register_index + 1U]);

        status = bq76940_cell_raw_to_mv(raw_count,
                                        &bq76940_calibration,
                                        &converted_cell_mv[cell_index]);
        if (status != BQ76940_STATUS_OK)
        {
            return status;
        }
    }

    for (uint8_t cell_index = 0U;
         cell_index < BQ76940_CELL_COUNT;
         ++cell_index)
    {
        cell_mv[cell_index] = converted_cell_mv[cell_index];
    }

    return BQ76940_STATUS_OK;
}
