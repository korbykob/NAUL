#include <gdt.h>
#include <serial.h>

const uint64_t gdt[3] =
{
    0x0000000000000000,
    0x00209A0000000000,
    0x0000920000000000
};
const struct {
    uint16_t length;
    uint64_t base;
} __attribute__((packed)) gdtr = { 23, (uint64_t)gdt };

__attribute__((naked)) void loadGdt()
{
    __asm__ volatile ("lgdt %0; pushq $0x08; leaq trampoline(%%rip), %%rax; pushq %%rax; retfq" : : "m"(gdtr));
    __asm__ volatile ("trampoline:");
    __asm__ volatile ("movw $0x10, %ax; movw %ax, %ds; movw %ax, %es; movw %ax, %fs; movw %ax, %gs; movw %ax, %ss; retq");
}

void initGdt()
{
    serialPrint("Loading GDT");
    loadGdt();
    serialPrint("Loaded GDT");
}
