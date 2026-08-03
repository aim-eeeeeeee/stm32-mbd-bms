#ifndef BSP_I2C_H
#define BSP_I2C_H

#include <stdint.h>

#include "bsp_status.h"

/**
 * @brief Check that the platform-configured I2C1 peripheral is ready.
 *
 * MX_I2C1_Init() must run before this function. This BSP function does not
 * configure clocks or pins a second time.
 */
bsp_status_t bsp_i2c_init(void);

/**
 * @brief Read one byte from an 8-bit device register.
 * @param device_address_7bit Unshifted 7-bit I2C device address.
 * @param register_address Device register address.
 * @param value Destination for the byte read.
 * @param timeout_ms Maximum blocking time in milliseconds; must be nonzero.
 */
bsp_status_t bsp_i2c_read_u8(uint8_t device_address_7bit,
                             uint8_t register_address,
                             uint8_t *value,
                             uint32_t timeout_ms);

/**
 * @brief Write one byte to an 8-bit device register.
 * @param device_address_7bit Unshifted 7-bit I2C device address.
 * @param register_address Device register address.
 * @param value Byte to write.
 * @param timeout_ms Maximum blocking time in milliseconds; must be nonzero.
 */
bsp_status_t bsp_i2c_write_u8(uint8_t device_address_7bit,
                              uint8_t register_address,
                              uint8_t value,
                              uint32_t timeout_ms);

/**
 * @brief Read consecutive bytes beginning at an 8-bit device register.
 * @param device_address_7bit Unshifted 7-bit I2C device address.
 * @param register_address First device register address.
 * @param data Destination buffer.
 * @param data_length Number of bytes to read; must be nonzero.
 * @param timeout_ms Maximum blocking time in milliseconds; must be nonzero.
 */
bsp_status_t bsp_i2c_read(uint8_t device_address_7bit,
                          uint8_t register_address,
                          uint8_t *data,
                          uint16_t data_length,
                          uint32_t timeout_ms);

/**
 * @brief Write consecutive bytes beginning at an 8-bit device register.
 * @param device_address_7bit Unshifted 7-bit I2C device address.
 * @param register_address First device register address.
 * @param data Source buffer.
 * @param data_length Number of bytes to write; must be nonzero.
 * @param timeout_ms Maximum blocking time in milliseconds; must be nonzero.
 */
bsp_status_t bsp_i2c_write(uint8_t device_address_7bit,
                           uint8_t register_address,
                           const uint8_t *data,
                           uint16_t data_length,
                           uint32_t timeout_ms);

#endif /* BSP_I2C_H */
