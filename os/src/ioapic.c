#include <ioapic.h>
#include <terminal.h>
#include <bootloader.h>
#include <io.h>

#define IOAPIC_SPURIOUS_REGISTER (APIC_BASE_ADDRESS + 0xF0)
#define IOAPIC_ID_REGISTER (APIC_BASE_ADDRESS + 0x20)
#define IOAPIC_MASTER_DATA 0x21
#define IOAPIC_SLAVE_DATA 0xA1
#define IOAPIC_ENABLE_BIT 0x100
#define IOAPIC_SPURIOUS_INTERRUPT 0xFF
#define IOAPIC_DATA_REGISTER 0x10
#define IOAPIC_REDIRECT_TABLE 0x10
#define IOAPIC_ID_MASK 0xFF000000

void initIoapic()
{
    log("Setting up IOAPIC");
    outb(IOAPIC_MASTER_DATA, __UINT8_MAX__);
    outb(IOAPIC_SLAVE_DATA, __UINT8_MAX__);
    log("Enabling the IOAPIC");
    *(uint32_t*)IOAPIC_SPURIOUS_REGISTER = IOAPIC_ENABLE_BIT | IOAPIC_SPURIOUS_INTERRUPT;
    log("Set up IOAPIC");
}

void writeIoapic(uint32_t index, uint32_t value)
{
    *(uint32_t*)(information.ioapicAddress) = index;
    *(uint32_t*)(information.ioapicAddress + IOAPIC_DATA_REGISTER) = value;
}

void unmaskIoapic(uint8_t interrupt)
{
    uint32_t index = IOAPIC_REDIRECT_TABLE + information.ioapicRedirects[interrupt] * 2;
    writeIoapic(index + 1, *(uint32_t*)IOAPIC_ID_REGISTER & IOAPIC_ID_MASK);
    writeIoapic(index, IOAPIC_OFFSET + interrupt);
}
