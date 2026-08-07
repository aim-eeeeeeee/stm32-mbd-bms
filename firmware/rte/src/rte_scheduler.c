#include "rte_scheduler.h"

#include "bq76940.h"

static measurement_snapshot_t rte_measurement_snapshot;
static uint32_t rte_last_run_ms;

void rte_scheduler_init(uint32_t now_ms)
{
    measurement_snapshot_init(&rte_measurement_snapshot);
    rte_model_adapter_init();
    rte_last_run_ms = now_ms - RTE_SCHEDULER_PERIOD_MS;
}

bool rte_scheduler_run(uint32_t now_ms)
{
    uint32_t elapsed_ms = now_ms - rte_last_run_ms;
    uint16_t cell_mv[MEASUREMENT_CELL_COUNT];
    bool stale;

    if (elapsed_ms < RTE_SCHEDULER_PERIOD_MS)
    {
        return false;
    }

    rte_last_run_ms = now_ms;

    if (bq76940_read_cell_voltages(cell_mv) == BQ76940_STATUS_OK)
    {
        (void)measurement_snapshot_publish(
            &rte_measurement_snapshot,
            cell_mv,
            now_ms);
    }
    else
    {
        measurement_snapshot_mark_source_error(&rte_measurement_snapshot);
    }

    stale = measurement_snapshot_is_stale(
        &rte_measurement_snapshot,
        now_ms,
        RTE_MEASUREMENT_STALE_MS);

    if (elapsed_ms > UINT16_MAX)
    {
        elapsed_ms = UINT16_MAX;
    }

    rte_model_adapter_step(
        &rte_measurement_snapshot,
        stale,
        (uint16_t)elapsed_ms);

    return true;
}

const measurement_snapshot_t *rte_scheduler_get_snapshot(void)
{
    return &rte_measurement_snapshot;
}

const rte_bms_output_t *rte_scheduler_get_bms_output(void)
{
    return rte_model_adapter_get_output();
}
