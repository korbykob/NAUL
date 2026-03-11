#pragma once

#include <definitions.h>

typedef struct
{
    uint16_t header;
    uint32_t size;
    uint32_t reserved;
    uint32_t offset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t depth;
    uint32_t compression;
    uint32_t imageSize;
    int32_t resolutionX;
    int32_t resolutionY;
    uint32_t paletteCount;
    uint32_t important;
} __attribute__((packed)) BmpHeader;

static void readBmp(BmpHeader* data, uint32_t* buffer)
{
    uint8_t* image = (uint8_t*)data + data->offset;
    for (int32_t y = 0; y < data->height; y++)
    {
        for (int32_t x = 0; x < data->width; x++)
        {
            buffer[(data->height - y - 1) * data->width + x] = (image[2] << 16) | (image[1] << 8) | image[0];
            image += 3;
        }
    }
}
