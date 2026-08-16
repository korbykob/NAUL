#pragma once

#include <str.h>
#include <mem.h>
#include <str.h>

#define strcmp(a, b) (int)compareStrings(a, b)
#define memcmp(a, b, length) (int)compareStart(a, b, length)
#define memcpy(destination, source, count) ({ copyMemory8((uint8_t*)(source), (uint8_t*)(destination), count); 0; })
#define memset(destination, value, count) ({ setMemory8((uint8_t*)(destination), value, count); 0; })
#define strcpy(destination, source) ({ copyString(source, destination); 0; })
