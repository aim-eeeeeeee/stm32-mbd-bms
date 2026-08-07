#include "rte_model_adapter.h"

#include <stddef.h>

#include "BmsSupervisor.h"

static rte_bms_output_t rte_bms_output;

void rte_model_adapter_init(void)
{
    BmsSupervisor_initialize();

    rte_bms_output.state = 0U;
    rte_bms_output.warning_flags = 0U;
    rte_bms_output.fault_flags = 0U;
    rte_bms_output.charge_allowed_shadow = false;
    rte_bms_output.discharge_allowed_shadow = false;
}

void rte_model_adapter_step(
    const measurement_snapshot_t *snapshot,
    bool measurement_stale,
    uint16_t step_ms)
{
    if (snapshot == NULL)
    {
        return;
    }

    BmsSupervisor_U.min_cell_mv = snapshot->min_cell_mv;
    BmsSupervisor_U.max_cell_mv = snapshot->max_cell_mv;
    BmsSupervisor_U.measurement_valid = snapshot->valid;
    BmsSupervisor_U.measurement_stale = measurement_stale;
    BmsSupervisor_U.step_ms = step_ms;

    BmsSupervisor_step();

    rte_bms_output.state = BmsSupervisor_Y.state;
    rte_bms_output.warning_flags = BmsSupervisor_Y.warning_flags;
    rte_bms_output.fault_flags = BmsSupervisor_Y.fault_flags;
    rte_bms_output.charge_allowed_shadow =
        BmsSupervisor_Y.charge_allowed_shadow;
    rte_bms_output.discharge_allowed_shadow =
        BmsSupervisor_Y.discharge_allowed_shadow;
}

const rte_bms_output_t *rte_model_adapter_get_output(void)
{
    return &rte_bms_output;
}
