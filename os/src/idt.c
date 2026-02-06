#include <idt.h>
#include <serial.h>
#include <panic.h>
#include <pic.h>

#define PIC_MASTER_SPURIOUS 7
#define PIC_SLAVE_SPURIOUS 15
#define KERNEL_CODE_SEGMENT 0x8
#define IDT_INTERRUPT_GATE 0xE
#define IDT_PRESENT 0x80

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
const struct {
    uint16_t length;
    uint64_t base;
} __attribute__((packed)) idtr = { sizeof(idt) - 1, (uint64_t)idt };
extern uint8_t decSize;
uint8_t exceptionNumber = 31;
uint64_t codeA = 0;
uint64_t codeB = 0;

void isr()
{
    __asm__ volatile ("isrStart:");
    if (exceptions[exceptionNumber].code)
    {
        __asm__ volatile ("pushq %0; jmp panic" : : "g"(codeB + JMP_SIZE), "D"((uint64_t)exceptionNumber), "S"(codeA));
    }
    else
    {
        __asm__ volatile ("pushq %0; jmp panic" : : "g"(codeA + JMP_SIZE), "D"((uint64_t)exceptionNumber));
    }
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
    __asm__ volatile ("movq (%%rsp), %0" : "=r"(codeA));
    __asm__ volatile ("movq 8(%%rsp), %0" : "=r"(codeB));
    __asm__ volatile ("jmp isrStart");
    __asm__ volatile ("decSize: .byte decEnd - exception");
}

__attribute__((naked)) void masterSpurious()
{
    __asm__ volatile ("iretq");
}

__attribute__((naked)) void slaveSpurious()
{
    __asm__ volatile ("pushw %ax; movb $0x20, %al; outb %al, $0x20; popw %ax; iretq");
}

void initIdt()
{
    serialPrint("Setting up IDT");
    for (uint8_t i = 0; i < EXCEPTION_COUNT; i++)
    {
        installIsr(i, (void (*)())((uint8_t*)exception + (i * decSize)));
    }
    installIrq(PIC_MASTER_SPURIOUS, masterSpurious);
    installIrq(PIC_SLAVE_SPURIOUS, slaveSpurious);
    serialPrint("Loading IDT");
    __asm__ volatile ("lidt %0" : : "m"(idtr));
    serialPrint("Set up IDT");
}

void installIsr(uint8_t interrupt, void (*handler)())
{
    uint16_t index = interrupt;
    idt[index].lower = (uint64_t)handler;
    idt[index].selector = KERNEL_CODE_SEGMENT;
    idt[index].ist = 0;
    idt[index].attributes = IDT_INTERRUPT_GATE | IDT_PRESENT;
    idt[index].middle = (uint64_t)handler >> 16;
    idt[index].higher = (uint64_t)handler >> 32;
    idt[index].zero = 0;
}

void installIrq(uint8_t interrupt, void (*handler)())
{
    installIsr(interrupt + PIC_OFFSET, handler);
}
