# BmsSupervisor Model Interface Contract

## Component and execution

| Property | Contract |
|---|---|
| Model name | `BmsSupervisor` |
| Model role | Application supervision and shadow protection decisions |
| Execution style | Initialize once, then step periodically |
| Model step period | 500 ms |
| Instances | One, nonreentrant instance for the MVP |
| Hardware access | None |

The RTE/model adapter shall write every input before one model step and copy
every output after that step. Inputs must not change while the step executes.

## Inputs

All ports are scalar.

| Name | Model type | Unit/scaling | Initial value | Expected/structural range | Meaning and source |
|---|---|---|---:|---|---|
| `min_cell_mv` | `uint16` | mV, 1 mV/bit | 0 | 1–6000 when valid | Minimum from the RTE-owned snapshot |
| `max_cell_mv` | `uint16` | mV, 1 mV/bit | 0 | 1–6000 when valid | Maximum from the same snapshot |
| `measurement_valid` | `boolean` | logical | false | false/true | Latest acquisition and publication succeeded |
| `measurement_stale` | `boolean` | logical | true | false/true | Last successful publication is too old |
| `step_ms` | `uint16` | ms, 1 ms/bit | 500 | 1–65535; expected 500 | Elapsed time represented by this model step |

When `measurement_valid` is false or `measurement_stale` is true, min/max may
contain retained last-good values. The quality flags determine whether the
model may use them for normal protection decisions.

## Outputs

All ports are scalar.

| Name | Model type | Unit/scaling | Initial value | Meaning and consumer |
|---|---|---|---:|---|
| `state` | `uint8` | enumerated code | `INIT` | BMS supervisor state; adapter then CAN telemetry |
| `warning_flags` | `uint16` | bit mask | 0 | Active warning categories; adapter then CAN telemetry |
| `fault_flags` | `uint16` | bit mask | 0 | Active fault categories; adapter then CAN telemetry |
| `charge_allowed_shadow` | `boolean` | logical | false | Observational charge permission only |
| `discharge_allowed_shadow` | `boolean` | logical | false | Observational discharge permission only |

The numeric state encoding and each flag-bit assignment are defined in
`BMS_SUPERVISOR_REQUIREMENTS.md`. Reserved values/bits shall decode as unknown
or inactive, not as a valid undocumented state.

## Ownership rules

1. The RTE owns the one `measurement_snapshot_t` object.
2. The measurement service is the only publisher of snapshot contents.
3. The adapter owns copying snapshot fields and derived staleness into model
   inputs.
4. Generated model code owns its internal states and generated I/O objects.
5. The adapter copies model outputs into one handwritten RTE output structure.
6. Only CAN telemetry consumes that output structure initially.
7. No output controls hardware in the MVP; the BMS remains in shadow mode.

## Quality, timing, and failure semantics

- `measurement_valid` and `measurement_stale` are independent signals.
- Invalid means the latest acquisition failed; stale means the last success is
  too old.
- Invalid/stale behavior shall be fail-safe: permissions default false until
  the model requirements explicitly allow recovery.
- All five inputs used in a step shall describe one RTE cycle.
- The adapter shall call `BmsSupervisor_initialize()` exactly once before the
  first `BmsSupervisor_step()`.
- Missed or delayed scheduling must be represented honestly through `step_ms`
  or handled by a later frozen scheduler rule; it must not be silently hidden.

## Expected generated-C boundary

For the first nonreentrant implementation, root Inports and Outports may use
the generated standard input/output structures, conceptually:

```c
BmsSupervisor_U.min_cell_mv = snapshot.min_cell_mv;
BmsSupervisor_U.max_cell_mv = snapshot.max_cell_mv;
BmsSupervisor_U.measurement_valid = snapshot.valid;
BmsSupervisor_U.measurement_stale = stale;
BmsSupervisor_U.step_ms = step_ms;

BmsSupervisor_step();

rte_output.state = BmsSupervisor_Y.state;
```

The exact generated type and symbol names must be verified after code
generation. Simulink `boolean` may appear through a generated Boolean typedef;
handwritten integration shall include generated headers rather than guessing
that representation.

## Format boundaries

- This internal contract uses engineering-unit integers, not packed CAN bytes.
- CPU byte order and CAN endianness do not change the model values; CAN packing
  is a separate interface contract created later.
- Warning and fault bit masks require a documented bit table before CAN use.
- No floating-point values, pointers, variable-length arrays, or platform HAL
  types cross the model boundary.

## Change control and resolved decisions

Changing a name, type, unit, dimension, sample period, state encoding, or flag
bit is an interface change. Update this file, model ports/data, adapter, tests,
and CAN mapping together.

The requirements checkpoint freezes the model step period, state encoding,
flag allocation, demonstration thresholds, timing, hysteresis, recovery, and
invalid/stale behavior. Any later change must update both contract documents,
the model, tests, generated-code integration, and CAN mapping.
