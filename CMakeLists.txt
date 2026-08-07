cmake_minimum_required(VERSION 3.22)

project(stm32-mbd-bms LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(FIRMWARE_TARGET stm32-mbd-bms)
set(LINKER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/firmware/bsw/platform/linker/STM32F103C8Tx_FLASH.ld"
)

set(PLATFORM_SOURCES
    firmware/bsw/platform/src/main.c
    firmware/bsw/platform/src/gpio.c
    firmware/bsw/platform/src/can.c
    firmware/bsw/platform/src/i2c.c
    firmware/bsw/platform/src/stm32f1xx_it.c
    firmware/bsw/platform/src/stm32f1xx_hal_msp.c
    firmware/bsw/platform/src/syscalls.c
    firmware/bsw/platform/src/sysmem.c
    firmware/bsw/platform/src/system_stm32f1xx.c
    firmware/bsw/platform/startup/startup_stm32f103xb.s
)

set(HAL_SOURCES
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
)

set(BSP_SOURCES
    firmware/bsw/bsp/src/bsp_time.c
    firmware/bsw/bsp/src/bsp_gpio.c
    firmware/bsw/bsp/src/bsp_i2c.c
    firmware/bsw/bsp/src/bsp_can.c
    firmware/bsw/bsp/src/bsp_watchdog.c
)

set(DRIVER_SOURCES
    firmware/bsw/drivers/src/bq76940.c
)

set(SERVICE_SOURCES
    firmware/bsw/services/src/measurement_snapshot.c
)

set(RTE_SOURCES
    firmware/rte/src/rte_model_adapter.c
    firmware/rte/src/rte_scheduler.c
)

set(ASW_SOURCES
    firmware/asw/bms_supervisor/src/generated/BmsSupervisor.c
)

add_executable(${FIRMWARE_TARGET}
    ${PLATFORM_SOURCES}
    ${BSP_SOURCES}
    ${DRIVER_SOURCES}
    ${SERVICE_SOURCES}
    ${RTE_SOURCES}
    ${ASW_SOURCES}
    ${HAL_SOURCES}
)

set_target_properties(${FIRMWARE_TARGET} PROPERTIES SUFFIX ".elf")

target_include_directories(${FIRMWARE_TARGET} PRIVATE
    firmware/bsw/platform/include
    firmware/bsw/bsp/include
    firmware/bsw/drivers/include
    firmware/bsw/services/include
    firmware/rte/include
    firmware/asw/bms_supervisor/src/generated
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc
    vendor/STM32CubeF1/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    vendor/STM32CubeF1/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    vendor/STM32CubeF1/Drivers/CMSIS/Include
)

target_compile_definitions(${FIRMWARE_TARGET} PRIVATE
    USE_HAL_DRIVER
    STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

set(CPU_OPTIONS
    -mcpu=cortex-m3
    -mthumb
    -mfloat-abi=soft
)

target_compile_options(${FIRMWARE_TARGET} PRIVATE
    ${CPU_OPTIONS}
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Wshadow>
    $<$<COMPILE_LANGUAGE:C>:-Wundef>
    $<$<COMPILE_LANGUAGE:C>:-Wformat=2>
    $<$<COMPILE_LANGUAGE:C>:-ffunction-sections>
    $<$<COMPILE_LANGUAGE:C>:-fdata-sections>
    $<$<COMPILE_LANGUAGE:C>:-fno-common>
    $<$<COMPILE_LANGUAGE:C>:-fstack-usage>
    $<$<COMPILE_LANGUAGE:ASM>:-x>
    $<$<COMPILE_LANGUAGE:ASM>:assembler-with-cpp>
)

target_link_options(${FIRMWARE_TARGET} PRIVATE
    ${CPU_OPTIONS}
    "-T${LINKER_SCRIPT}"
    --specs=nano.specs
    -Wl,--gc-sections
    -Wl,--cref
    -Wl,--print-memory-usage
    "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${FIRMWARE_TARGET}.map"
)

target_link_libraries(${FIRMWARE_TARGET} PRIVATE m)

set_property(TARGET ${FIRMWARE_TARGET} APPEND PROPERTY
    LINK_DEPENDS "${LINKER_SCRIPT}"
)

set(FIRMWARE_HEX "${CMAKE_CURRENT_BINARY_DIR}/${FIRMWARE_TARGET}.hex")
set(FIRMWARE_BIN "${CMAKE_CURRENT_BINARY_DIR}/${FIRMWARE_TARGET}.bin")

add_custom_command(TARGET ${FIRMWARE_TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:${FIRMWARE_TARGET}> ${FIRMWARE_HEX}
    COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${FIRMWARE_TARGET}> ${FIRMWARE_BIN}
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${FIRMWARE_TARGET}>
    BYPRODUCTS ${FIRMWARE_HEX} ${FIRMWARE_BIN}
    COMMENT "Generating HEX/BIN files and reporting firmware size"
    VERBATIM
)
