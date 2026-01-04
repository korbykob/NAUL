#include <idt.h>
#include <serial.h>
#include <kernel.h>

struct
{
    uint16_t lower;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t middle;
    uint32_t higher;
    uint32_t zero;
} __attribute__((packed)) idt[256];
struct {
    uint16_t length;
    uint64_t base;
} __attribute__((packed)) idtr = { 4095, (uint64_t)idt };

extern uint8_t decSize;
uint8_t exceptionNumber = 31;

void isr()
{
    panic(exceptionNumber);
}

__attribute__((naked)) void exception()
{
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decEnd = .");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("decb exceptionNumber(%rip)");
    __asm__ volatile ("jmp isr");
    __asm__ volatile ("decSize: .byte decEnd - exception");
}

void initIdt()
{
    serialPrint("Setting up IDT");
    for (uint8_t i = 0; i < 32; i++)
    {
        installIsr(i, (void (*)())((uint8_t*)exception + (i * decSize)));
    }
    serialPrint("Loading IDT");
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    serialPrint("Loaded IDT");
}

void installIsr(uint8_t interrupt, void (*handler)())
{
    uint16_t index = interrupt;
    idt[index].lower = (uint64_t)handler;
    idt[index].selector = 0x08;
    idt[index].ist = 0;
    idt[index].attributes = 0x8E;
    idt[index].middle = (uint64_t)handler >> 16;
    idt[index].higher = (uint64_t)handler >> 32;
    idt[index].zero = 0;
}

void installIrq(uint8_t interrupt, void (*handler)())
{
    installIsr(interrupt + 0x20, handler);
}
