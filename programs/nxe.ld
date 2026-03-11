OUTPUT_FORMAT(binary)
ENTRY(entry)
SECTIONS
{
    . = 0x8000000000 - 8;

    .header :
    {
        QUAD(entry);
    }

    .text ALIGN(16) :
    {
        *(.text*)
    }

    .rodata ALIGN(16) :
    {
        *(.rodata*)
    }

    .lrodata ALIGN(16) :
    {
        *(.lrodata*)
    }

    .data ALIGN(16) :
    {
        *(.data*)
    }

    .ldata ALIGN(16) :
    {
        *(.ldata*)
    }

    .bss ALIGN(16) :
    {
        *(.bss*)
        BYTE(0);
    }
    
    .lbss ALIGN(16) :
    {
        *(.lbss*)
        BYTE(0);
    }
}
