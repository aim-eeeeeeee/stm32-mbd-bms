#include "measurement_snapshot.h"

#include <stddef.h>
#include <string.h>

void measurement_snapshot_init(measurement_snapshot_t *snapshot)
{
    if (snapshot == NULL)
    {
        return;
    }

    memset(snapshot, 0, sizeof(*snapshot));
}

measurement_status_t measurement_snapshot_publish(
    measurement_snapshot_t *snapshot,
    const uint16_t cell_mv[MEASUREMENT_CELL_COUNT],
    uint32_t now_ms)
{
    if (snapshot == NULL)
    {
        return MEASUREMENT_STATUS_INVALID_ARG;
    }

    if (cell_mv == NULL)
    {
        snapshot->valid = false;
        return MEASUREMENT_STATUS_INVALID_ARG;
    }

    for (uint8_t cell_index = 0U;
         cell_index < MEASUREMENT_CELL_COUNT;
         ++cell_index)
    {
        if ((cell_mv[cell_index] < MEASUREMENT_CELL_MV_MIN_PLAUSIBLE) ||
            (cell_mv[cell_index] > MEASUREMENT_CELL_MV_MAX_PLAUSIBLE))
        {
            snapshot->valid = false;
            return MEASUREMENT_STATUS_INVALID_CELL;
        }
    }
    measurement_snapshot_t candidate = {0};

    candidate.min_cell_mv = cell_mv[0];
    candidate.max_cell_mv = cell_mv[0];

    for (uint8_t cell_index = 0U;
         cell_index < MEASUREMENT_CELL_COUNT;
         ++cell_index)
    {
        candidate.cell_mv[cell_index] = cell_mv[cell_index];

        if (cell_mv[cell_index] < candidate.min_cell_mv)
        {
            candidate.min_cell_mv = cell_mv[cell_index];
        }

        if (cell_mv[cell_index] > candidate.max_cell_mv)
        {
            candidate.max_cell_mv = cell_mv[cell_index];
        }
    }

    candidate.last_success_ms = now_ms;
    candidate.sequence = snapshot->sequence + 1U;
    if (candidate.sequence == 0U)
    {
        candidate.sequence = 1U;
    }
    candidate.valid = true;

    *snapshot = candidate;
    return MEASUREMENT_STATUS_OK;
}

void measurement_snapshot_mark_source_error(measurement_snapshot_t *snapshot)
{
    if (snapshot == NULL)
    {
        return;
    }

    snapshot->valid = false;
}

bool measurement_snapshot_is_stale(
    const measurement_snapshot_t *snapshot,
    uint32_t now_ms,
    uint32_t stale_after_ms)
{
    uint32_t age_ms;

    if ((snapshot == NULL) ||
        (stale_after_ms == 0U) ||
        (snapshot->sequence == 0U))
    {
        return true;
    }

    age_ms = now_ms - snapshot->last_success_ms;
    return (age_ms >= stale_after_ms);
}
