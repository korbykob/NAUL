#include <gdt.h>
#include <definitions.h>
#include <serial.h>

#define GDT_LONG_MODE 0x20000000000000
#define GDT_PRESENT 0x800000000000
#define GDT_CODE_DATA 0x100000000000
#define GDT_EXECUTABLE 0x80000000000
#define GDT_READ_WRITE 0x20000000000

const uint64_t gdt[3] =
{
    0x0,
    GDT_PRESENT | GDT_LONG_MODE | GDT_CODE_DATA | GDT_EXECUTABLE | GDT_READ_WRITE,
    GDT_PRESENT | GDT_CODE_DATA | GDT_READ_WRITE
};
const struct {
    uint16_t length;
    uint64_t base;
} __attribute__((packed)) gdtr = { sizeof(gdt) - 1, (uint64_t)gdt };

__attribute__((naked)) void loadGdt()
{
    __asm__ volatile ("lgdt %0; pushq $0x08; leaq trampoline(%%rip), %%rax; pushq %%rax; retfq" : : "m"(gdtr) : "%rax");
    __asm__ volatile ("trampoline:");
    __asm__ volatile ("movw $0x10, %ax; movw %ax, %ds; movw %ax, %es; movw %ax, %fs; movw %ax, %gs; movw %ax, %ss; retq");
}

void initGdt()
{
    serialPrint("Setting up GDT");
    loadGdt();
    serialPrint("Set up GDT");
}
