#ifndef RTE_MODEL_ADAPTER_H
#define RTE_MODEL_ADAPTER_H

#include <stdbool.h>
#include <stdint.h>

#include "measurement_snapshot.h"

typedef struct {
    uint8_t state;
    uint16_t warning_flags;
    uint16_t fault_flags;
    bool charge_allowed_shadow;
    bool discharge_allowed_shadow;
} rte_bms_output_t;

void rte_model_adapter_init(void);

void rte_model_adapter_step(
    const measurement_snapshot_t *snapshot,
    bool measurement_stale,
    uint16_t step_ms);

const rte_bms_output_t *rte_model_adapter_get_output(void);

#endif /* RTE_MODEL_ADAPTER_H */
