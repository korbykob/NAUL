#pragma once

#include <definitions.h>

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint32_t flags;
    uint32_t count;
    uint32_t glyphSize;
    uint32_t height;
    uint32_t width;
    uint8_t data[];
} PsfFile;
