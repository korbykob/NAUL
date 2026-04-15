#pragma once

#define yieldThread() __asm__ volatile ("int $0x67" : : : "memory")

static inline void lock(bool* mutex)
{
    __asm__ volatile ("movb $1, %%bl; mutexCheck%=:; xorb %%al, %%al; lock cmpxchgb %%bl, %0; je mutexLocked%=; int $0x67; jmp mutexCheck%=; mutexLocked%=:" : "+m"(*mutex) : : "%al", "%bl", "memory");
}

static inline void unlock(bool* mutex)
{
    __asm__ volatile ("lock andb $0, %0" : "+m"(*mutex) : : "memory");
}
