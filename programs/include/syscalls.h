#pragma once

#define SYSCALL_0(number) \
{\
    __asm__ volatile ("int $0x69" : : "D"(number) : "%rax"); \
}

#define SYSCALL_0_RETURN(number, type) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("int $0x69" : "=a"(result) : "D"(number)); \
    return (type)result; \
}

#define SYSCALL_1(number, arg1) \
{\
    __asm__ volatile ("int $0x69" : : "D"(number), "S"(arg1) : "%rax"); \
}

#define SYSCALL_1_RETURN(number, type, arg1) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("int $0x69" : "=a"(result) : "D"(number), "S"(arg1)); \
    return (type)result; \
}

#define SYSCALL_2(number, arg1, arg2) \
{\
    __asm__ volatile ("int $0x69" : : "D"(number), "S"(arg1), "d"(arg2) : "%rax"); \
}

#define SYSCALL_2_RETURN(number, type, arg1, arg2) \
{\
    uint64_t result = 0; \
    __asm__ volatile ("int $0x69" : "=a"(result) : "D"(number), "S"(arg1), "d"(arg2)); \
    return (type)result; \
}
