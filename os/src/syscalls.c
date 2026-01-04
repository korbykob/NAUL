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
    __asm__ volatile ("pushq %rbx; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq %rbp; pushq %rsp; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15; subq $256, %rsp; movdqu %xmm0, 240(%rsp); movdqu %xmm1, 224(%rsp); movdqu %xmm2, 208(%rsp); movdqu %xmm3, 192(%rsp); movdqu %xmm4, 176(%rsp); movdqu %xmm5, 160(%rsp); movdqu %xmm6, 144(%rsp); movdqu %xmm7, 128(%rsp); movdqu %xmm8, 112(%rsp); movdqu %xmm9, 96(%rsp); movdqu %xmm10, 80(%rsp); movdqu %xmm11, 64(%rsp); movdqu %xmm12, 48(%rsp); movdqu %xmm13, 32(%rsp); movdqu %xmm14, 16(%rsp); movdqu %xmm15, (%rsp)");
    __asm__ volatile ("sti; cld; call syscallHandler");
    __asm__ volatile ("movdqu (%rsp), %xmm15; movdqu 16(%rsp), %xmm14; movdqu 32(%rsp), %xmm13; movdqu 48(%rsp), %xmm12; movdqu 64(%rsp), %xmm11; movdqu 80(%rsp), %xmm10; movdqu 96(%rsp), %xmm9; movdqu 112(%rsp), %xmm8; movdqu 128(%rsp), %xmm7; movdqu 144(%rsp), %xmm6; movdqu 160(%rsp), %xmm5; movdqu 176(%rsp), %xmm4; movdqu 192(%rsp), %xmm3; movdqu 208(%rsp), %xmm2; movdqu 224(%rsp), %xmm1; movdqu 240(%rsp), %xmm0; addq $256, %rsp; popq %r15; popq %r14; popq %r13; popq %r12; popq %r11; popq %r10; popq %r9; popq %r8; popq %rsp; popq %rbp; popq %rdi; popq %rsi; popq %rdx; popq %rcx; popq %rbx");
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
