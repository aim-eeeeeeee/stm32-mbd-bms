#ifndef BSP_STATUS_H
#define BSP_STATUS_H

/**
 * @brief Status values returned by board-support package interfaces.
 */
typedef enum {
    BSP_STATUS_SUCCESS = 0,     // success
    BSP_STATUS_INVALID_ARG,     // invalid argument
    BSP_STATUS_NOT_INIT,        // not initialized
    BSP_STATUS_BUSY,            // busy
    BSP_STATUS_TIMEOUT,         // timeout
    BSP_STATUS_COMM_ERR,        // communication error
    BSP_STATUS_GENERIC_HW_ERR,  // generic hardware error
} bsp_status_t;

#endif /* BSP_STATUS_H */