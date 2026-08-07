#ifndef MEASUREMENT_SNAPSHOT_H
#define MEASUREMENT_SNAPSHOT_H

#include <stdbool.h>
#include <stdint.h>

#define MEASUREMENT_CELL_COUNT              (15U)
#define MEASUREMENT_CELL_MV_MIN_PLAUSIBLE   (1U)
#define MEASUREMENT_CELL_MV_MAX_PLAUSIBLE   (6000U)

typedef enum {
    MEASUREMENT_STATUS_OK = 0,
    MEASUREMENT_STATUS_INVALID_ARG,
    MEASUREMENT_STATUS_INVALID_CELL
} measurement_status_t;

typedef struct {
    uint16_t cell_mv[MEASUREMENT_CELL_COUNT];
    uint16_t min_cell_mv;
    uint16_t max_cell_mv;
    uint32_t last_success_ms;
    uint32_t sequence;
    bool valid;
} measurement_snapshot_t;

/**
 * @brief Reset a measurement snapshot to its never-published state.
 *
 * All stored measurements, metadata, and the valid flag are cleared. Passing
 * NULL has no effect.
 *
 * @param snapshot Snapshot to initialize, or NULL.
 */
void measurement_snapshot_init(measurement_snapshot_t *snapshot);

/**
 * @brief Validate and publish one complete set of cell measurements.
 *
 * On success, all cell values, min/max, timestamp, sequence, and validity are
 * published together. On failure, the last good measurement fields remain
 * unchanged and the snapshot is marked invalid when snapshot is non-NULL.
 *
 * @param snapshot Destination snapshot.
 * @param cell_mv Complete array of cell voltages in millivolts.
 * @param now_ms Current monotonic time in milliseconds.
 * @return MEASUREMENT_STATUS_OK on success, or an argument/cell-data error.
 */
measurement_status_t measurement_snapshot_publish(
    measurement_snapshot_t *snapshot,
    const uint16_t cell_mv[MEASUREMENT_CELL_COUNT],
    uint32_t now_ms);

/**
 * @brief Mark the latest acquisition as failed without erasing last-good data.
 *
 * Only the valid flag is cleared. Passing NULL has no effect.
 *
 * @param snapshot Snapshot whose source has failed, or NULL.
 */
void measurement_snapshot_mark_source_error(
    measurement_snapshot_t *snapshot);

/**
 * @brief Determine whether no sufficiently recent successful sample exists.
 *
 * Staleness is calculated with wrap-safe unsigned millisecond subtraction.
 * A NULL snapshot, zero stale interval, or never-published snapshot is stale.
 *
 * @param snapshot Snapshot to inspect.
 * @param now_ms Current monotonic time in milliseconds.
 * @param stale_after_ms Age at which a last-good sample becomes stale.
 * @return true when stale or unusable for an age check; otherwise false.
 */
bool measurement_snapshot_is_stale(
    const measurement_snapshot_t *snapshot,
    uint32_t now_ms,
    uint32_t stale_after_ms);

#endif /* MEASUREMENT_SNAPSHOT_H */
