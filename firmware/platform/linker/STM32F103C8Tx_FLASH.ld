/*
 * GNU linker script for the STM32F103C8T6.
 *
 * Physical memory:
 *   Flash: 0x08000000 - 0x0800FFFF (64 KiB)
 *   SRAM:  0x20000000 - 0x20004FFF (20 KiB)
 *
 * The legacy firmware stores configuration at 0x0800FC00, so the final
 * 1 KiB flash page is deliberately excluded from the application image.
 */

ENTRY(Reset_Handler)

_Min_Heap_Size = 0x200;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

MEMORY
{
    FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 63K
    CONFIG (rx)  : ORIGIN = 0x0800FC00, LENGTH = 1K
    RAM    (xrw) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Symbols available to C code if the reserved page is managed later. */
__config_page_start__ = ORIGIN(CONFIG);
__config_page_end__   = ORIGIN(CONFIG) + LENGTH(CONFIG);

SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _etext = .;
    } > FLASH

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > FLASH

    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > FLASH

    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > FLASH

    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array*))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

    /*
     * Initial values for .data live in flash. Reset_Handler copies them to
     * RAM using _sidata, _sdata, and _edata.
     */
    _sidata = LOADADDR(.data);

    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM AT> FLASH

    /*
     * .bss occupies RAM but has no bytes in the firmware image.
     * Reset_Handler clears this range using _sbss and _ebss.
     */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
    } > RAM

    .heap_stack (NOLOAD) :
    {
        . = ALIGN(8);
        __end__ = .;
        end = .;
        . += _Min_Heap_Size;
        . += _Min_Stack_Size;
        . = ALIGN(8);
    } > RAM

    /DISCARD/ :
    {
        *(.note.GNU-stack)
        *(.gnu_debuglink)
        *(.gnu.lto_*)
    }
}

ASSERT(ORIGIN(FLASH) + LENGTH(FLASH) == ORIGIN(CONFIG),
       "Application flash must end at the reserved configuration page")

ASSERT(ORIGIN(CONFIG) + LENGTH(CONFIG) == 0x08010000,
       "Configuration page must end at the STM32F103C8 flash boundary")

ASSERT(_edata <= ORIGIN(RAM) + LENGTH(RAM),
       "Initialized data exceeds STM32F103C8 RAM")

ASSERT(__bss_end__ + _Min_Heap_Size + _Min_Stack_Size <= _estack,
       "RAM overflow: data, BSS, heap, and minimum stack do not fit")
