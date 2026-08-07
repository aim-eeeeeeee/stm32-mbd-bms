# BmsSupervisor smoke tests

Run the deterministic scripted harness from the repository root:

```matlab
addpath('firmware/asw/bms_supervisor/model/scripts');
run_model_tests();
```

The runner loads `BmsSupervisor_Harness.slx`, replaces its Constant sources
in memory with typed time-series inputs, logs all five model outputs, and
checks startup, normal, invalid, stale, and quality-recovery behavior. The
saved harness is not changed.

These smoke tests intentionally do not validate UV/OV warning and fault timing.
Passing them is sufficient for the current integration exercise, but is not
evidence that the complete protection requirements are verified.

Code generation calls this suite first and stops if any scenario fails:

```matlab
generate_code();
```

MATLAB cache and code-generation workspaces are redirected under the
repository-level `codegen/` directory. Deployable generated sources are kept
separately under `src/generated`.
