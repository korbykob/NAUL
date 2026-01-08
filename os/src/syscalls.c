#include <syscalls.h>
#include <serial.h>
#include <idt.h>

uint64_t (*syscallHandlers[256])(uint64_t arg1, uint64_t arg2, uint64_t arg3);

uint64_t syscallHandler(uint64_t code, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
    return syscallHandlers[code](arg1, arg2, arg3);
}

__attribute__((naked)) void syscallTrampoline()
{
    __asm__ volatile ("pushq %rbx; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq %rbp; pushq %rsp; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15");
    __asm__ volatile ("sti; cld; call syscallHandler");
    __asm__ volatile ("popq %r15; popq %r14; popq %r13; popq %r12; popq %r11; popq %r10; popq %r9; popq %r8; popq %rsp; popq %rbp; popq %rdi; popq %rsi; popq %rdx; popq %rcx; popq %rbx");
    __asm__ volatile ("iretq");
}

void initSyscalls()
{
    serialPrint("Setting up syscalls");
    installIsr(0x69, syscallTrampoline);
    serialPrint("Set up syscalls");
}

void registerSyscall(uint64_t code, void *handler)
{
    syscallHandlers[code] = (uint64_t (*)(uint64_t arg1, uint64_t arg2, uint64_t arg3))handler;
}
