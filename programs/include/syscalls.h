#pragma once

#define SYSCALL_0(number) \
{\
    __asm__ volatile ("movq %0, %%rdi; int $0x69" : : "i"(number) : "%rdi", "%rax"); \
}

#define SYSCALL_0_RETURN(number, type) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("movq %1, %%rdi; int $0x69; movq %%rax, %0" : "=g"(result) : "i"(number) : "%rdi", "%rax"); \
    return (type)result; \
}

#define SYSCALL_1(number, arg1) \
{\
    __asm__ volatile ("movq %0, %%rdi; movq %1, %%rsi; int $0x69" : : "i"(number) , "g"(arg1) : "%rdi", "%rsi", "%rax"); \
}

#define SYSCALL_1_RETURN(number, type, arg1) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("movq %1, %%rdi; movq %2, %%rsi; int $0x69; movq %%rax, %0" : "=g"(result) : "i"(number), "g"(arg1) : "%rdi", "%rsi", "%rax"); \
    return (type)result; \
}

#define SYSCALL_2(number, arg1, arg2) \
{\
    __asm__ volatile ("movq %0, %%rdi; movq %1, %%rsi; movq %2, %%rdx; int $0x69" : : "i"(number) , "g"(arg1), "g"(arg2) : "%rdi", "%rsi", "%rdx", "%rax"); \
}

#define SYSCALL_2_RETURN(number, type, arg1, arg2) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("movq %1, %%rdi; movq %2, %%rsi; movq %3, %%rdx; int $0x69; movq %%rax, %0" : "=g"(result) : "i"(number), "g"(arg1), "g"(arg2) : "%rdi", "%rsi", "%rdx", "%rax"); \
    return (type)result; \
}
