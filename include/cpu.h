#pragma once

#define pushRegisters() __asm__ volatile ("pushq %rax; pushq %rbx; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq %rbp; pushq %rsp; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15")
#define pushAvxRegisters() __asm__ volatile ("subq $512, %rsp; vmovdqu %ymm0, 480(%rsp); vmovdqu %ymm1, 448(%rsp); vmovdqu %ymm2, 416(%rsp); vmovdqu %ymm3, 384(%rsp); vmovdqu %ymm4, 352(%rsp); vmovdqu %ymm5, 320(%rsp); vmovdqu %ymm6, 288(%rsp); vmovdqu %ymm7, 256(%rsp); vmovdqu %ymm8, 224(%rsp); vmovdqu %ymm9, 192(%rsp); vmovdqu %ymm10, 160(%rsp); vmovdqu %ymm11, 128(%rsp); vmovdqu %ymm12, 96(%rsp); vmovdqu %ymm13, 64(%rsp); vmovdqu %ymm14, 32(%rsp); vmovdqu %ymm15, (%rsp)")
#define popRegisters() __asm__ volatile ("popq %r15; popq %r14; popq %r13; popq %r12; popq %r11; popq %r10; popq %r9; popq %r8; popq %rsp; popq %rbp; popq %rdi; popq %rsi; popq %rdx; popq %rcx; popq %rbx; popq %rax")
#define popAvxRegisters() __asm__ volatile ("vmovdqu (%rsp), %ymm15; vmovdqu 32(%rsp), %ymm14; vmovdqu 64(%rsp), %ymm13; vmovdqu 96(%rsp), %ymm12; vmovdqu 128(%rsp), %ymm11; vmovdqu 160(%rsp), %ymm10; vmovdqu 192(%rsp), %ymm9; vmovdqu 224(%rsp), %ymm8; vmovdqu 256(%rsp), %ymm7; vmovdqu 288(%rsp), %ymm6; vmovdqu 320(%rsp), %ymm5; vmovdqu 352(%rsp), %ymm4; vmovdqu 384(%rsp), %ymm3; vmovdqu 416(%rsp), %ymm2; vmovdqu 448(%rsp), %ymm1; vmovdqu 480(%rsp), %ymm0; addq $512, %rsp")

static inline void lock(bool* mutex)
{
    __asm__ volatile ("movb $1, %%bl; mutexCheck%=:; xorb %%al, %%al; lock cmpxchgb %%bl, %0; je mutexLocked%=; int $0x67; jmp mutexCheck%=; mutexLocked%=:" : "+m"(*mutex) : : "%al", "%bl", "memory");
}

static inline void unlock(bool* mutex)
{
    __asm__ volatile ("lock andb $0, %0" : "+m"(*mutex) : : "memory");
}
