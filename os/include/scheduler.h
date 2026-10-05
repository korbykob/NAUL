#pragma once

#include <definitions.h>
#include <symbols.h>

#define APIC_BASE_ADDRESS 0xFEE00000
#define LAPIC_EOI_REGISTER (APIC_BASE_ADDRESS + 0xB0)
#define pushCr3() __asm__ volatile ("movq %%cr3, %%rax; pushq %%rax" : : : "%rax")
#define popCr3() __asm__ volatile ("popq %%rax; movq %%rax, %%cr3" : : : "%rax")
#define pushRegisters() __asm__ volatile ("pushq %rax; pushq %rbx; pushq %rcx; pushq %rdx; pushq %rsi; pushq %rdi; pushq %rbp; pushq %rsp; pushq %r8; pushq %r9; pushq %r10; pushq %r11; pushq %r12; pushq %r13; pushq %r14; pushq %r15")
#define popRegisters() __asm__ volatile ("popq %r15; popq %r14; popq %r13; popq %r12; popq %r11; popq %r10; popq %r9; popq %r8; popq %rsp; popq %rbp; popq %rdi; popq %rsi; popq %rdx; popq %rcx; popq %rbx; popq %rax")
#ifndef NO_AVX2
    #define pushSimdRegisters() __asm__ volatile ("subq $512, %rsp; vmovdqu %ymm0, 480(%rsp); vmovdqu %ymm1, 448(%rsp); vmovdqu %ymm2, 416(%rsp); vmovdqu %ymm3, 384(%rsp); vmovdqu %ymm4, 352(%rsp); vmovdqu %ymm5, 320(%rsp); vmovdqu %ymm6, 288(%rsp); vmovdqu %ymm7, 256(%rsp); vmovdqu %ymm8, 224(%rsp); vmovdqu %ymm9, 192(%rsp); vmovdqu %ymm10, 160(%rsp); vmovdqu %ymm11, 128(%rsp); vmovdqu %ymm12, 96(%rsp); vmovdqu %ymm13, 64(%rsp); vmovdqu %ymm14, 32(%rsp); vmovdqu %ymm15, (%rsp)")
    #define popSimdRegisters() __asm__ volatile ("vmovdqu (%rsp), %ymm15; vmovdqu 32(%rsp), %ymm14; vmovdqu 64(%rsp), %ymm13; vmovdqu 96(%rsp), %ymm12; vmovdqu 128(%rsp), %ymm11; vmovdqu 160(%rsp), %ymm10; vmovdqu 192(%rsp), %ymm9; vmovdqu 224(%rsp), %ymm8; vmovdqu 256(%rsp), %ymm7; vmovdqu 288(%rsp), %ymm6; vmovdqu 320(%rsp), %ymm5; vmovdqu 352(%rsp), %ymm4; vmovdqu 384(%rsp), %ymm3; vmovdqu 416(%rsp), %ymm2; vmovdqu 448(%rsp), %ymm1; vmovdqu 480(%rsp), %ymm0; addq $512, %rsp")
#else
    #define pushSimdRegisters() __asm__ volatile ("subq $256, %rsp; movdqu %xmm0, 240(%rsp); movdqu %xmm1, 224(%rsp); movdqu %xmm2, 208(%rsp); movdqu %xmm3, 192(%rsp); movdqu %xmm4, 176(%rsp); movdqu %xmm5, 160(%rsp); movdqu %xmm6, 144(%rsp); movdqu %xmm7, 128(%rsp); movdqu %xmm8, 112(%rsp); movdqu %xmm9, 96(%rsp); movdqu %xmm10, 80(%rsp); movdqu %xmm11, 64(%rsp); movdqu %xmm12, 48(%rsp); movdqu %xmm13, 32(%rsp); movdqu %xmm14, 16(%rsp); movdqu %xmm15, (%rsp)")
    #define popSimdRegisters() __asm__ volatile ("movdqu (%rsp), %xmm15; movdqu 16(%rsp), %xmm14; movdqu 32(%rsp), %xmm13; movdqu 48(%rsp), %xmm12; movdqu 64(%rsp), %xmm11; movdqu 80(%rsp), %xmm10; movdqu 96(%rsp), %xmm9; movdqu 112(%rsp), %xmm8; movdqu 128(%rsp), %xmm7; movdqu 144(%rsp), %xmm6; movdqu 160(%rsp), %xmm5; movdqu 176(%rsp), %xmm4; movdqu 192(%rsp), %xmm3; movdqu 208(%rsp), %xmm2; movdqu 224(%rsp), %xmm1; movdqu 240(%rsp), %xmm0; addq $256, %rsp")
#endif

typedef struct
{
    void* next;
    void* prev;
    uint64_t id;
    uint64_t waiting;
    Symbol* symbols;
    uint64_t symbolCount;
    uint8_t ttyId;
    uint8_t* stack;
    uint64_t sp;
} Thread;

extern Thread* currentThread;

void initScheduler();

uint64_t createThread(void (*function)());

void waitForThread(uint64_t id);

void destroyThread(uint64_t id);

void exitThread();
