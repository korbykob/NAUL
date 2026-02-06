#include <pic.h>
#include <serial.h>
#include <io.h>

#define PIC_MASTER_COMMAND 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_COMMAND 0xA0
#define PIC_SLAVE_DATA 0xA1
#define PIC_INTERRUPT_COUNT 8
#define PIC_INIT 0x10
#define PIC_FOUR_COMMANDS 0x1
#define PIC_MASTER_CASCADE 0x04
#define PIC_SLAVE_CASCADE 2
#define PIC_8086_MODE 0x01
#define PIC_EOI 0x20

void initPic()
{
    serialPrint("Setting up PICs");
    outb(PIC_MASTER_COMMAND, PIC_INIT | PIC_FOUR_COMMANDS);
    outb(PIC_SLAVE_COMMAND, PIC_INIT | PIC_FOUR_COMMANDS);
    serialPrint("Setting IRQ offsets");
    outb(PIC_MASTER_DATA, PIC_OFFSET);
    outb(PIC_SLAVE_DATA, PIC_OFFSET + PIC_INTERRUPT_COUNT);
    serialPrint("Setting up second PIC at IRQ 2");
    outb(PIC_MASTER_DATA, PIC_MASTER_CASCADE);
    outb(PIC_SLAVE_DATA, PIC_SLAVE_CASCADE);
    serialPrint("Place PICs into 8086 mode");
    outb(PIC_MASTER_DATA, PIC_8086_MODE);
    outb(PIC_SLAVE_DATA, PIC_8086_MODE);
    serialPrint("Masking all interrupts");
    outb(PIC_MASTER_DATA, __UINT8_MAX__);
    outb(PIC_SLAVE_DATA, __UINT8_MAX__);
    serialPrint("Unmasking cascade interrupt");
    unmaskPic(PIC_SLAVE_CASCADE);
    serialPrint("Enabling interrupts");
    __asm__ volatile ("sti");
    serialPrint("Set up PICs");
}

void unmaskPic(uint8_t interrupt)
{
    if (interrupt < PIC_INTERRUPT_COUNT)
    {
        outb(PIC_MASTER_DATA, inb(PIC_MASTER_DATA) & ~(1 << interrupt));
    }
    else
    {
        outb(PIC_SLAVE_DATA, inb(PIC_SLAVE_DATA) & ~(1 << (interrupt - PIC_INTERRUPT_COUNT)));
    }
}

void maskPic(uint8_t interrupt)
{
    if (interrupt < PIC_INTERRUPT_COUNT)
    {
        outb(PIC_MASTER_DATA, inb(PIC_MASTER_DATA) | (1 << interrupt));
    }
    else
    {
        outb(PIC_SLAVE_DATA, inb(PIC_SLAVE_DATA) | (1 << (interrupt - PIC_INTERRUPT_COUNT)));
    }
}

void picAck(uint8_t interrupt)
{
    outb(PIC_MASTER_COMMAND, PIC_EOI);
    if (interrupt >= PIC_INTERRUPT_COUNT)
    {
        outb(PIC_SLAVE_COMMAND, PIC_EOI);
    }
}
