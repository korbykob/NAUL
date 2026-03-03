#pragma once

#define SYSCALL_0(number) \
{\
    __asm__ volatile ("int $0x69" : : "D"(number) : "%rax", "memory"); \
}

#define SYSCALL_0_RETURN(number, type) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("int $0x69" : "=a"(result) : "D"(number) : "memory"); \
    return (type)result; \
}

#define SYSCALL_1(number, arg1) \
{\
    __asm__ volatile ("int $0x69" : : "D"(number), "S"(arg1) : "%rax", "memory"); \
}

#define SYSCALL_1_RETURN(number, type, arg1) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("int $0x69" : "=a"(result) : "D"(number), "S"(arg1) : "memory"); \
    return (type)result; \
}

#define SYSCALL_2(number, arg1, arg2) \
{\
    __asm__ volatile ("int $0x69" : : "D"(number), "S"(arg1), "d"(arg2) : "%rax", "memory"); \
}

#define SYSCALL_2_RETURN(number, type, arg1, arg2) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("int $0x69" : "=a"(result) : "D"(number), "S"(arg1), "d"(arg2) : "memory"); \
    return (type)result; \
}

#define SYSCALL_3(number, arg1, arg2, arg3) \
{\
    __asm__ volatile ("int $0x69" : : "D"(number), "S"(arg1), "d"(arg2), "c"(arg3) : "%rax", "memory"); \
}

#define SYSCALL_3_RETURN(number, type, arg1, arg2, arg3) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("int $0x69" : "=a"(result) : "D"(number), "S"(arg1), "d"(arg2), "c"(arg3) : "memory"); \
    return (type)result; \
}

#define SYSCALL_4(number, arg1, arg2, arg3, arg4) \
{\
    __asm__ volatile ("movq %4, %%r8; int $0x69" : : "D"(number), "S"(arg1), "d"(arg2), "c"(arg3), "g"(arg4) : "%rax", "memory"); \
}

#define SYSCALL_4_RETURN(number, type, arg1, arg2, arg3, arg4) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("movq %5, %%r8; int $0x69" : "=a"(result) : "D"(number), "S"(arg1), "d"(arg2), "c"(arg3), "g"(arg4) : "memory"); \
    return (type)result; \
}

#define SYSCALL_5(number, arg1, arg2, arg3, arg4, arg5) \
{\
    __asm__ volatile ("movq %4, %%r8; movq %5, %%r9; int $0x69" : : "D"(number), "S"(arg1), "d"(arg2), "c"(arg3), "g"(arg4), "g"(arg5) : "%rax", "memory"); \
}

#define SYSCALL_5_RETURN(number, type, arg1, arg2, arg3, arg4, arg5) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("movq %5, %%r8; movq %6, %%r9; int $0x69" : "=a"(result) : "D"(number), "S"(arg1), "d"(arg2), "c"(arg3), "g"(arg4), "g"(arg5) : "memory"); \
    return (type)result; \
}
