# STM32 BMS

A battery management system prototype for an STM32F103 and BQ76940-based
board. It measures cell voltages, validates measurement health, evaluates
charge/discharge protection, manages the BMS operating state, and reports the
result over CAN.

The required end-to-end path is:

```text
Cell input
  → BQ76940 driver
  → validated measurement
  → MBD protection and state logic
  → CAN diagnostics
  → TSMaster
```

The initial system operates in **shadow mode**: it measures, evaluates, and
reports warnings, faults, and CHG/DSG permissions without controlling the
MOSFETs. It is an engineering prototype, not a production-ready or
safety-certified BMS.

## Tech Stack

| Layer | Technology | Purpose |
|---|---|---|
| Hardware | STM32F103, BQ76940 | Processing and cell measurement |
| Platform | CMSIS, STM32CubeF1 HAL | Startup, GPIO, I2C, CAN, timers |
| Drivers | Board Support Package (BSP), BQ76940 driver | Isolate hardware and decode AFE data |
| Services | Scheduler, measurement, diagnostics | Validate data and coordinate tasks |
| Application | MATLAB/Simulink, generated C | BMS states, warnings, faults, hysteresis |
| Communication | CAN, TSMaster | Report measurements and model state |
| Build | CMake, Ninja, Arm GNU Toolchain | Configure and compile firmware |

```text
BQ76940 → HAL → BSP → Driver → Services → MBD Logic → CAN → TSMaster
```

## Overview

```mermaid
flowchart BT
    subgraph Hardware["Hardware"]
        CELLS[Battery Cells]
        MCU[STM32F103]
        AFE[BQ76940 AFE]
        BUS[CAN Bus]
        TS[TSMaster]
    end

    subgraph Platform["Low-Level Platform"]
        direction LR
        CORE[CMSIS / Startup / Linker]
        HAL[STM32CubeF1 HAL]
        CLOCK[Clock / Timer]
        WD[Watchdog]
        GPIO[GPIO / Pin Init]
        HALI2C[HAL I2C]
        HALCAN[HAL CAN]
    end

    subgraph Drivers["Board Support and Drivers"]
        direction LR
        BSPGPIO[BSP GPIO]
        BSPTIME[BSP Time]
        BSPI2C[BSP I2C]
        BSPCAN[BSP CAN]
        BQ[BQ76940 Driver]
    end

    subgraph Services["Firmware Services"]
        direction LR
        SC[Cooperative Scheduler]
        MS[Measurement Service]
        DB[BMS Database]
        DS[Diagnostic Service]
        CP[CAN Protocol Encoder]
    end

    subgraph Application["Application"]
        direction LR
        MAIN[main.c]
        MA[Model Adapter]
    end

    subgraph MBD["MBD BMS Logic"]
        direction LR
        MV["Measurement Supervision<br/>validity and stale-data handling"]
        PL["CHG/DSG Protection<br/>warnings, faults, and permissions"]
        SM["BMS State Machine<br/>startup, normal, warning, fault, sensor error"]
    end

    subgraph Development["Development Environment"]
        direction LR
        MATLAB[MATLAB / Simulink]
        BUILD[CMake / Ninja / Arm GNU]
    end

    CELLS --> AFE
    MCU --> CORE --> HAL
    HAL --> CLOCK
    HAL --> WD
    HAL --> GPIO --> BSPGPIO
    HAL --> HALI2C --> BSPI2C --> BQ
    HAL --> HALCAN --> BSPCAN
    AFE --> BQ

    CLOCK --> BSPTIME --> SC
    BQ --> MS --> DB --> MA
    BSPGPIO --> MAIN
    WD --> MAIN
    SC --> MS
    SC --> MA
    MAIN --> SC
    MAIN --> BQ

    MA --> MV --> PL --> SM
    SM --> DS
    PL --> DS
    DB --> DS
    DS --> CP --> BSPCAN
    BSPCAN --> BUS --> TS

    MATLAB -. creates .-> MBD
    BUILD -. builds .-> MAIN
```
