#include <power.h>
#include <serial.h>
#include <bootloader.h>
#include <scheduler.h>
#include <hpet.h>
#include <syscalls.h>
#include <calls.h>
#include <io.h>
#include <cpu.h>
#include <str.h>

#define POWER_SMI_COMMAND 0x30
#define POWER_ENABLE_REGISTER 0x34
#define POWER_RESET_SPACE 0x74
#define POWER_RESET_ADDRESS 0x78
#define POWER_RESET_VALUE 0x80
#define POWER_PS2_COMMAND 0x64
#define POWER_PS2_RESET 0xFE
#define POWER_FADT_DSDT 0x4
#define POWER_DSDT_LENGTH 0x4
#define POWER_HEADER_SIZE 36
#define POWER_PM1A_EVENT 0x38
#define POWER_PM1A_CONTROL 0x40
#define POWER_PM1B_CONTROL 0x44
#define POWER_SLEEP_ENABLE 0x2000
#define POWER_POWER_BUTTON 0x100
#define POWER_BOCHS_PORT 0xB004
#define POWER_BOCHS_VALUE 0x2000
#define POWER_QEMU_PORT 0x604
#define POWER_QEMU_VALUE 0x2000
#define POWER_VBOX_PORT 0x4004
#define POWER_VBOX_VALUE 0x3400
#define POWER_CLOUD_PORT 0x600
#define POWER_CLOUD_VALUE 0x34

uint16_t shutdownA = 0;
uint16_t shutdownB = 0;

void powerThread()
{
    uint64_t last = getFemtoseconds();
    while (true)
    {
        uint64_t femtoseconds = getFemtoseconds();
        if (femtoseconds - last >= FEMTOSECONDS_PER_MILLISECOND * 100)
        {
            last = femtoseconds;
            if (inw(*(uint32_t*)(information.fadtAddress + POWER_PM1A_EVENT)) & POWER_POWER_BUTTON)
            {
                outw(*(uint32_t*)(information.fadtAddress + POWER_PM1A_EVENT), POWER_POWER_BUTTON);
                shutdown();
            }
        }
        yieldThread();
    }
}

void initPower()
{
    serialPrint("Setting up power");
    registerSyscall(REBOOT, reboot);
    registerSyscall(SHUTDOWN, shutdown);
    serialPrint("Enabling ACPI");
    outb(*(uint32_t*)(information.fadtAddress + POWER_SMI_COMMAND), *(uint8_t*)(information.fadtAddress + POWER_ENABLE_REGISTER));
    serialPrint("Searching for S5 mode");
    bool guess = true;
    char* current = (char*)((uint64_t)*(uint32_t*)(information.fadtAddress + POWER_FADT_DSDT) + POWER_HEADER_SIZE);
    char* end = current + *(uint32_t*)((uint64_t)*(uint32_t*)(information.fadtAddress + POWER_FADT_DSDT) + POWER_DSDT_LENGTH);
    while (current < end)
    {
        if (compareStart(current, "_S5_", 4) == 0)
        {
            current += 4;
            if (*current == 0x12)
            {
                current += 3;
                if (*current == 0xA)
                {
                    current++;
                }
                shutdownA = *(uint8_t*)current << 10;
                current++;
                if (*current == 0xA)
                {
                    current++;
                }
                shutdownB = *(uint8_t*)current << 10;
                guess = false;
                break;
            }
        }
        current++;
    }
    if (guess)
    {
        shutdownA = 0x1400;
    }
    serialPrint("Creating power thread");
    createThread(powerThread);
    serialPrint("Set up power");
}

void reboot()
{
    if (*(uint8_t*)(information.fadtAddress + POWER_RESET_SPACE))
    {
        outb(*(uint64_t*)(information.fadtAddress + POWER_RESET_ADDRESS), *(uint8_t*)(information.fadtAddress + POWER_RESET_VALUE));
    }
    else
    {
        *(uint8_t*)(*(uint64_t*)(information.fadtAddress + POWER_RESET_ADDRESS)) = *(uint8_t*)(information.fadtAddress + POWER_RESET_VALUE);
    }
    outb(POWER_PS2_COMMAND, POWER_PS2_RESET);
}

void shutdown()
{
    outw(*(uint32_t*)(information.fadtAddress + POWER_PM1A_CONTROL), shutdownA | POWER_SLEEP_ENABLE);
    uint32_t value = *(uint32_t*)(information.fadtAddress + POWER_PM1B_CONTROL);
    if (value)
    {
        outw(value, shutdownB | POWER_SLEEP_ENABLE);
    }
    outw(POWER_BOCHS_PORT, POWER_BOCHS_VALUE);
    outw(POWER_QEMU_PORT, POWER_QEMU_VALUE);
    outw(POWER_VBOX_PORT, POWER_VBOX_VALUE);
    outw(POWER_CLOUD_PORT, POWER_CLOUD_VALUE);
}
