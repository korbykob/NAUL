OUTPUT_FORMAT(binary)
ENTRY(entry)
SECTIONS
{
    . = 0x8000000000 - (__header_end - __header_start);

    __header_start = .;
    .header :
    {
        QUAD(entry - ADDR(.header));
    }
    __header_end = .;

    .text ALIGN(16) :
    {
        *(.text*)
    }

    .rodata ALIGN(16) :
    {
        *(.rodata*)
    }

    .data ALIGN(16) :
    {
        *(.data*)
        
        . = ALIGN(16);
        
        *(.bss*)
        *(COMMON)
    }
}
