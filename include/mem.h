#pragma once

#include <definitions.h>

static inline void setMemory8(uint8_t* destination, uint8_t value, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        *destination++ = value;
    }
}

static inline void setMemory16(uint16_t* destination, uint16_t value, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        *destination++ = value;
    }
}

static inline void setMemory32(uint32_t* destination, uint32_t value, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        *destination++ = value;
    }
}

static inline void setMemory64(uint64_t* destination, uint64_t value, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        *destination++ = value;
    }
}

static inline void copyMemory8(uint8_t* source, uint8_t* destination, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        *destination++ = *source++;
    }
}

static inline void copyMemory16(uint16_t* source, uint16_t* destination, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        *destination++ = *source++;
    }
}

static inline void copyMemory32(uint32_t* source, uint32_t* destination, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        *destination++ = *source++;
    }
}

static inline void copyMemory64(uint64_t* source, uint64_t* destination, uint64_t count)
{
    for (uint64_t i = 0; i < count; i++)
    {
        *destination++ = *source++;
    }
}
