#pragma once

#include <definitions.h>

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t value = 0;
    __asm__ volatile ("inb %w1, %b0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

static inline void outw(uint16_t port, uint16_t value)
{
    __asm__ volatile ("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t value = 0;
    __asm__ volatile ("inw %w1, %w0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}

static inline void outd(uint16_t port, uint32_t value)
{
    __asm__ volatile ("outl %k0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint32_t ind(uint16_t port)
{
    uint32_t value = 0;
    __asm__ volatile ("inl %w1, %k0" : "=a"(value) : "Nd"(port) : "memory");
    return value;
}
