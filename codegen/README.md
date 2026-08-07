# MATLAB code-generation workspace

`generate_code.m` writes the complete Simulink/Embedded Coder workspace here.
This directory is intentionally outside `firmware/` because it contains both
deployable C code and temporary MATLAB build metadata.

Only reviewed C/H dependencies needed by the STM32 build belong in:

```text
firmware/asw/bms_supervisor/src/generated/
```

Do not add `.obj`, `.rsp`, `.mk`, `.bat`, `.mat`, `.dmr`, `.tmw`, cache files,
or a generated `ert_main.c` to the firmware sources.
