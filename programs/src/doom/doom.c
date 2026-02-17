#include <definitions.h>
#include <allocator.h>
#include <filesystem.h>
#include <tty.h>
#include <hpet.h>
#include <keyboard.h>
#include <mouse.h>
#include <processes.h>
#include <display.h>

#define DOOM_IMPLEMENTATION
#include "PureDOOM.h"

#define MAX_FILES_OPEN 5
#define MICROSECONDS_PER_SECOND 1000000
#define FRAMEBUFFER_SCALING 4

struct
{
    uint8_t* memory;
    uint64_t size;
    uint64_t offset;
} openFiles[MAX_FILES_OPEN];
uint64_t last = 0;
int useconds = 0;
int seconds = 0;
KeyboardBuffer keyboardBuffer = { 0, 0 };
MouseBuffer mouseBuffer = { 0, 0 };
const char codes[256] = {
    0, DOOM_KEY_ESCAPE, DOOM_KEY_1, DOOM_KEY_2, DOOM_KEY_3, DOOM_KEY_4, DOOM_KEY_5, DOOM_KEY_6, DOOM_KEY_7, DOOM_KEY_8, /* 9 */
    DOOM_KEY_9, DOOM_KEY_0, DOOM_KEY_MINUS, DOOM_KEY_EQUALS, DOOM_KEY_BACKSPACE, /* Backspace */
    DOOM_KEY_TAB, /* Tab */
    DOOM_KEY_Q, DOOM_KEY_W, DOOM_KEY_E, DOOM_KEY_R, /* 19 */
    DOOM_KEY_T, DOOM_KEY_Y, DOOM_KEY_U, DOOM_KEY_U, DOOM_KEY_O, DOOM_KEY_P, DOOM_KEY_LEFT_BRACKET, DOOM_KEY_RIGHT_BRACKET, DOOM_KEY_ENTER, /* Enter key */
    DOOM_KEY_CTRL, /* 29 - Control */
    DOOM_KEY_A, DOOM_KEY_S, DOOM_KEY_D, DOOM_KEY_F, DOOM_KEY_G, DOOM_KEY_H, DOOM_KEY_J, DOOM_KEY_K, DOOM_KEY_L, DOOM_KEY_SEMICOLON, /* 39 */
    DOOM_KEY_APOSTROPHE, 0, DOOM_KEY_SHIFT, /* Left shift */
    0, DOOM_KEY_Z, DOOM_KEY_X, DOOM_KEY_C, DOOM_KEY_V, DOOM_KEY_B, DOOM_KEY_N, /* 49 */
    DOOM_KEY_M, DOOM_KEY_COMMA, DOOM_KEY_PERIOD, DOOM_KEY_SLASH, DOOM_KEY_SHIFT, /* Right shift */
    DOOM_KEY_MULTIPLY, DOOM_KEY_ALT, /* Alt */
    DOOM_KEY_SPACE, /* Space bar */
    0, /* Caps lock */
    DOOM_KEY_F1, /* 59 - F1 key ... > */
    DOOM_KEY_F2, DOOM_KEY_F3, DOOM_KEY_F4, DOOM_KEY_F5, DOOM_KEY_F6, DOOM_KEY_F7, DOOM_KEY_F8, DOOM_KEY_F9, DOOM_KEY_F10, /* < ... F10 */
    0, /* 69 - Num lock */
    0, /* Scroll Lock */
    0, /* Home key */
    DOOM_KEY_UP_ARROW, /* Up Arrow */
    0, /* Page Up */
    DOOM_KEY_MINUS, DOOM_KEY_LEFT_ARROW, /* Left Arrow */
    0, DOOM_KEY_RIGHT_ARROW, /* Right Arrow */
    0, 0, /* 79 - End key */
    DOOM_KEY_DOWN_ARROW, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0, DOOM_KEY_F11, /* F11 Key */
    DOOM_KEY_F12, /* F12 Key */
    0 /* All other keys are undefined */
};

void* doomAlloc(int size)
{
    return allocate(size);
}

void* doomOpen(const char* filename, const char* mode)
{
    if (checkFile(filename))
    {
        uint64_t id = 0;
        while (openFiles[id].memory)
        {
            id++;
        }
        openFiles[id].offset = 0;
        openFiles[id].memory = getFile(filename, &openFiles[id].size);
        return (void*)(id + 1);
    }
    return 0;
}

void doomClose(void* handle)
{
    openFiles[(uint64_t)handle - 1].memory = 0;
}

int doomRead(void* handle, void *buf, int count)
{
    int bytes = min(openFiles[(uint64_t)handle - 1].offset + count, openFiles[(uint64_t)handle - 1].size) - openFiles[(uint64_t)handle - 1].offset;
    uint8_t* source = openFiles[(uint64_t)handle - 1].memory + openFiles[(uint64_t)handle - 1].offset;
    uint8_t* destination = (uint8_t*)buf;
    for (int i = 0; i < bytes; i++)
    {
        *destination++ = *source++;
    }
    openFiles[(uint64_t)handle - 1].offset += bytes;
    return bytes;
}

int doomWrite(void* handle, const void *buf, int count)
{
    return -1;
}

int doomSeek(void* handle, int offset, doom_seek_t origin)
{
    openFiles[(uint64_t)handle - 1].offset = offset;
    return offset;
}

int doomTell(void* handle)
{
    return -1;
}

int doomEof(void* handle)
{
    return 1;
}

void doomTime(int* sec, int* usec)
{
    uint64_t new = getFemtoseconds();
    useconds += (new - last) / FEMTOSECONDS_PER_MICROSECOND;
    while (useconds >= MICROSECONDS_PER_SECOND)
    {
        useconds -= MICROSECONDS_PER_SECOND;
        seconds++;
    }
    last = new;
    *usec = useconds;
    *sec = seconds;
}

void doomExit(int code)
{
    unregisterKeyboard(&keyboardBuffer);
    unregisterMouse(&mouseBuffer);
    releaseDisplay();
    quit();
}

char* doomEnv(const char* var)
{
    return "/programs/doom";
}

void entry()
{
    for (uint8_t i = 0; i < MAX_FILES_OPEN; i++)
    {
        openFiles[i].memory = 0;
    }
    doom_set_print(write);
    doom_set_malloc(doomAlloc, unallocate);
    doom_set_file_io(doomOpen, doomClose, doomRead, doomWrite, doomSeek, doomTell, doomEof);
    doom_set_gettime(doomTime);
    doom_set_exit(doomExit);
    doom_set_getenv(doomEnv);
    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_use", DOOM_KEY_E);
    doom_set_default_int("mouse_move", 0);
    doom_init(0, 0, 0);
    Display display;
    obtainDisplay(&display);
    registerKeyboard(&keyboardBuffer);
    registerMouse(&mouseBuffer);
    bool lastLeft = false;
    bool lastRight = false;
    while (true)
    {
        while (keyboardBuffer.tail != keyboardBuffer.head)
        {
            uint8_t key = codes[keyboardBuffer.buffer[keyboardBuffer.tail].scancode];
            if (key)
            {
                if (keyboardBuffer.buffer[keyboardBuffer.tail].pressed)
                {
                    doom_key_down(key);
                }
                else
                {
                    doom_key_up(key);
                }
            }
            keyboardBuffer.tail++;
        }
        while (mouseBuffer.tail != mouseBuffer.head)
        {
            if (mouseBuffer.buffer[mouseBuffer.tail].x != 0 || mouseBuffer.buffer[mouseBuffer.tail].y != 0)
            {
                doom_mouse_move(mouseBuffer.buffer[mouseBuffer.tail].x * 20, mouseBuffer.buffer[mouseBuffer.tail].y * -20);
            }
            if (mouseBuffer.buffer[mouseBuffer.tail].left != lastLeft)
            {
                lastLeft = mouseBuffer.buffer[mouseBuffer.tail].left;
                if (lastLeft)
                {
                    doom_button_down(DOOM_LEFT_BUTTON);
                }
                else
                {
                    doom_button_up(DOOM_LEFT_BUTTON);
                }
            }
            if (mouseBuffer.buffer[mouseBuffer.tail].right != lastRight)
            {
                lastRight = mouseBuffer.buffer[mouseBuffer.tail].right;
                if (lastRight)
                {
                    doom_button_down(DOOM_RIGHT_BUTTON);
                }
                else
                {
                    doom_button_up(DOOM_RIGHT_BUTTON);
                }
            }
            mouseBuffer.tail++;
        }
        doom_update();
        uint32_t* buffer = display.buffer;
        uint32_t* doomVideo = (uint32_t*)doom_get_framebuffer(4);
        uint8_t skipX = 0;
        uint8_t skipY = 0;
        for (uint32_t y = 0; y < SCREENHEIGHT * FRAMEBUFFER_SCALING; y++)
        {
            for (uint32_t x = 0; x < SCREENWIDTH * FRAMEBUFFER_SCALING; x++)
            {
                *buffer++ = *doomVideo;
                skipX++;
                if (skipX == FRAMEBUFFER_SCALING)
                {
                    skipX = 0;
                    doomVideo++;
                }
            }
            skipY++;
            if (skipY == FRAMEBUFFER_SCALING)
            {
                skipY = 0;
            }
            else
            {
                doomVideo -= SCREENWIDTH;
            }
            buffer += display.width - SCREENWIDTH * FRAMEBUFFER_SCALING;
        }
    }
}
