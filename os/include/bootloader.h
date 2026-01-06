#pragma once

#include <definitions.h>

typedef struct
{
    char* name;
    uint64_t size;
    uint8_t* data;
} InitFile;
typedef struct
{
    uint32_t* framebuffer;
    uint32_t width;
    uint32_t height;
    InitFile* fileData;
    uint64_t fileCount;
    uint64_t hpetAddress;
} Info;

extern Info information;
