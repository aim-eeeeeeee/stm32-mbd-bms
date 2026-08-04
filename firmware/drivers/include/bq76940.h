/**
 * @file bq76940.h
 * @brief Read-only bring-up interface for the fitted BQ7694003 battery AFE.
 *
 * Hardware and protocol reference (verified from the IC marking and TI
 * BQ769x0 datasheet SLUSBK2I, Revision I, March 2022):
 *
 * - Fitted marking: 21AV7VTG4 / BQ7694003.
 * - Target variant: BQ7694003DBT, 9-to-15-series-cell BQ76940 family member.
 * - Address passed to the BSP: 0x08 (unshifted 7-bit address).
 * - Address byte on the wire: 0x10 for write and 0x11 for read.
 * - The BSP owns the one-bit shift required by STM32 HAL. This driver passes
 *   0x08 to the BSP and must not shift it again.
 * - Variant interface/LDO voltage: 3.3 V.
 * - CRC is enabled: polynomial x^8 + x^2 + x + 1 (0x07), initial value 0.
 * - A CRC byte follows each data byte. On block reads, the first CRC covers
 *   the read address byte plus data byte 0; later CRCs cover their data only.
 * - SYS_STAT is 0x00 and contains write-one-to-clear status bits. Days 4-5
 *   read it but do not clear it.
 * - VC1_HI 0x0C through VC15_LO 0x29 contain 30 logical bytes, requiring 60
 *   received wire bytes with CRC enabled.
 * - Calibration: ADCGAIN1 0x50, ADCOFFSET 0x51, ADCGAIN2 0x59.
 */

#ifndef BQ76940_H
#define BQ76940_H

#include <stdint.h>

#define BQ76940_CELL_COUNT (15U)

typedef enum {
    BQ76940_STATUS_OK = 0,
    BQ76940_STATUS_INVALID_ARG,
    BQ76940_STATUS_NOT_INIT,
    BQ76940_STATUS_TRANSPORT_ERR,
    BQ76940_STATUS_TIMEOUT,
    BQ76940_STATUS_CRC_ERR,
    BQ76940_STATUS_INVALID_DATA,
} bq76940_status_t;

typedef struct {
    int16_t adc_gain_uv_per_lsb;
    int8_t adc_offset_mv;
} bq76940_calibration_t;

/**
 * @brief Verify communication and load the fitted AFE's factory calibration.
 *
 * This is a single, fixed-device interface for the BQ7694003 at 7-bit address
 * 0x08. The function checks the existing BSP/I2C configuration, reads status,
 * and validates ADC calibration. It does not boot the AFE, clear faults, or
 * enable ADC, balancing, protection, CHG, or DSG controls.
 *
 * @return BQ76940_STATUS_OK on success, or an error status. The driver remains
 *         uninitialized if any required operation fails.
 */
bq76940_status_t bq76940_init(void);

/**
 * @brief Copy the factory ADC calibration cached during initialization.
 *
 * @param calibration Destination for gain and offset calibration.
 * @return BQ76940_STATUS_OK on success, BQ76940_STATUS_INVALID_ARG if
 *         calibration is NULL, or BQ76940_STATUS_NOT_INIT before successful
 *         initialization.
 *
 * @note The destination remains unchanged if the function fails.
 */
bq76940_status_t bq76940_get_calibration(bq76940_calibration_t *calibration);

/**
 * @brief Read SYS_STAT without clearing any latched status bits.
 *
 * @param system_status Destination for the raw SYS_STAT byte.
 * @return BQ76940_STATUS_OK on success, or an argument, transport, timeout, or
 *         CRC error status.
 *
 * @note The destination remains unchanged if the function fails. This function
 *       performs no write-one-to-clear operation.
 */
bq76940_status_t bq76940_read_status(uint8_t *system_status);

/**
 * @brief Read and convert all 15 cell voltages.
 *
 * @param cell_mv Output array with capacity for BQ76940_CELL_COUNT values.
 * @return BQ76940_STATUS_OK on success, or an error status.
 *
 * @note The output array remains unchanged if the function fails.
 */
bq76940_status_t bq76940_read_cell_voltages(uint16_t cell_mv[BQ76940_CELL_COUNT]);

#endif /* BQ76940_H */
