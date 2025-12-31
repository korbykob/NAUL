#pragma once

#include <definitions.h>

static uint32_t* graphicsBuffer = 0;
static uint32_t graphicsPitch = 0;
static uint8_t* graphicsFont = 0;

static void initGraphics(uint32_t* buffer, uint32_t pitch, uint8_t* font)
{
    graphicsBuffer = buffer;
    graphicsPitch = pitch;
    graphicsFont = font;
}

static void blit(uint32_t* source, uint32_t* destination, uint32_t width, uint32_t height)
{
    uint64_t* to = (uint64_t*)destination;
    uint64_t* from = (uint64_t*)source;
    for (uint64_t i = 0; i < (width * height) / 2; i++)
    {
        *to++ = *from++;
    }
}

static void fillScreen(uint32_t* destination, uint32_t colour, uint32_t width, uint32_t height)
{
    uint64_t* to = (uint64_t*)destination;
    for (uint64_t i = 0; i < (width * height) / 2; i++)
    {
        *to++ = colour;
    }
}

static void drawRectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour)
{
    uint32_t colours[2];
    colours[0] = colour;
    colours[1] = colour;
    uint64_t* address = (uint64_t*)(graphicsBuffer + y * graphicsPitch + x);
    uint32_t drop = (graphicsPitch - width) / 2;
    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width / 2; x++)
        {
            *address++ = *(uint64_t*)colours;
        }
        address += drop;
    }
}

static void drawImage(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t* buffer)
{
    uint64_t* from = (uint64_t*)buffer;
    uint64_t* to = (uint64_t*)(graphicsBuffer + y * graphicsPitch + x);
    uint64_t drop = (graphicsPitch - width) / 2;
    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width / 2; x++)
        {
            *to++ = *from++;
        }
        to += drop;
    }
}

static void drawCharacter(char character, uint32_t x, uint32_t y, uint32_t colour)
{
    uint32_t* address = graphicsBuffer + y * graphicsPitch + x;
    uint8_t* glyph = graphicsFont + 32 + 64 * character;
    for (uint8_t y = 0; y < 64; y += 2)
    {
        for (uint8_t x = 0; x < 8; x++)
        {
            if (glyph[y] & (0b10000000 >> x))
            {
                *address = colour;
            }
            address++;
        }
        for (uint8_t x = 0; x < 8; x++)
        {
            if (glyph[y + 1] & (0b10000000 >> x))
            {
                *address = colour;
            }
            address++;
        }
        address += graphicsPitch - 16;
    }
}

static void drawString(const char* string, uint32_t x, uint32_t y, uint32_t colour)
{
    uint32_t* address = graphicsBuffer + y * graphicsPitch + x;
    while (*string != 0)
    {
        uint8_t* glyph = graphicsFont + 32 + 64 * *string++;
        for (uint8_t y = 0; y < 32; y++)
        {
            for (uint8_t x = 0; x < 8; x++)
            {
                if (*glyph & (0b10000000 >> x))
                {
                    *address = colour;
                }
                address++;
            }
            glyph++;
            for (uint8_t x = 0; x < 8; x++)
            {
                if (*glyph & (0b10000000 >> x))
                {
                    *address = colour;
                }
                address++;
            }
            glyph++;
            address += graphicsPitch - 16;
        }
        address -= graphicsPitch * 32 - 16;
    }
}
