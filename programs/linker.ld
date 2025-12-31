OUTPUT_FORMAT(binary)
ENTRY(entry)
SECTIONS
{
    . = 0x8000000000 - (__header_end - __header_start);

    __header_start = .;
    .header :
    {
        LONG(entry - ADDR(.header));
    }
    __header_end = .;

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
        *(COMMON)
        *(.bss*)
    }
}
