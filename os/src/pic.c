#include <pic.h>
#include <serial.h>
#include <io.h>

void initPic()
{
    serialPrint("Setting up PICs");
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    serialPrint("Setting IRQ offsets");
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    serialPrint("Setting up second PIC at IRQ 2");
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    serialPrint("Place PICs into 8086 mode");
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    serialPrint("Masking all interrupts");
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
    serialPrint("Enabling interrupts");
    __asm__ volatile ("sti");
    serialPrint("Set up PICs");
}

void unmaskPic(uint8_t interrupt)
{
    if (interrupt < 8)
    {
        outb(0x21, inb(0x21) & ~(1 << interrupt));
    }
    else
    {
        outb(0xA1, inb(0xA1) & ~(1 << (interrupt - 8)));
    }
}

void picAck(uint8_t interrupt)
{
    outb(0x20, 0x20);
    if (interrupt >= 8)
    {
        outb(0xA0, 0x20);
    }
}
