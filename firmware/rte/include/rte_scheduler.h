#ifndef RTE_SCHEDULER_H
#define RTE_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#include "measurement_snapshot.h"
#include "rte_model_adapter.h"

#define RTE_SCHEDULER_PERIOD_MS       (500U)
#define RTE_MEASUREMENT_STALE_MS      (1000U)

void rte_scheduler_init(uint32_t now_ms);
bool rte_scheduler_run(uint32_t now_ms);
const measurement_snapshot_t *rte_scheduler_get_snapshot(void);
const rte_bms_output_t *rte_scheduler_get_bms_output(void);

#endif /* RTE_SCHEDULER_H */
