# Firmware architecture

The firmware uses an AUTOSAR-inspired separation without implementing full
AUTOSAR:

```text
firmware/
  asw/   Application software, including reviewed generated model code
  rte/   Cooperative scheduling and handwritten model/data adapters
  bsw/   STM32 platform, BSP, device drivers, and measurement services
```

Dependency direction is `ASW <- RTE -> BSW`. Generated ASW must not call the
HAL, BSP, or device drivers. The RTE owns the measurement snapshot and is the
only layer that maps BSW data into generated model inputs.

`asw/bms_supervisor/generated` contains only generated sources required by the
embedded build. MATLAB build metadata and host executables remain outside the
firmware tree.
