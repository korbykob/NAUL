#pragma once

#include <stddef.h>
#include <str.h>
#include <mem.h>

#define strcmp(a, b) (int)compareStrings(a, b)
#define memcmp(a, b, length) (int)compareStart(a, b, length)
#define memcpy(destination, source, count) ({ copyMemory8((uint8_t*)(source), (uint8_t*)(destination), count); 0; })
