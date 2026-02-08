#include <terminal.h>
#include <serial.h>
#include <bootloader.h>
#include <filesystem.h>
#include <allocator.h>
#include <hpet.h>
#include <scheduler.h>
#include <syscalls.h>
#include <paging.h>
#include <keyboard.h>
#include <display.h>
#include <calls.h>
#include <cpu.h>
#include <mem.h>
#include <psf.h>

#define TERMINAL_DEFAULT 0x989898
#define TERMINAL_WHITE 0xFFFFFF
#define TERMINAL_BLUE 0x0080FF
#define TERMINAL_GREEN 0x008000
#define TERMINAL_NO_CHAR '\xff'
#define TERMINAL_COLOUR '\xfe'
#define KEY_LEFT_SHIFT 42
#define KEY_RIGHT_SHIFT 54
#define KEY_CAPS_LOCK 58
#define KEY_BACKSPACE 14
#define KEY_ENTER 28

PsfFile* font = 0;
uint32_t fontWidth = 0;
uint32_t fontHeight = 0;
char* backBuffer = 0;
uint32_t terminalWidth = 0;
uint32_t terminalHeight = 0;
uint32_t terminalPitch = 0;
uint32_t cursorX = 0;
uint32_t cursorY = 0;
const uint32_t colours[] = { TERMINAL_DEFAULT, TERMINAL_WHITE, TERMINAL_BLUE, TERMINAL_GREEN };
uint8_t colour = 0;
bool writing = false;
const char scancodes[256] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', 0, /* Backspace */
    0, /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, /* Enter key */
    0, /* 29 - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`', 0, /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', /* 49 */
    'm', ',', '.', '/', 0, /* Right shift */
    '*', 0, /* Alt */
    ' ', /* Space bar */
    0, /* Caps lock */
    0, /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0, 0, /* < ... F10 */
    0, /* 69 - Num lock */
    0, /* Scroll Lock */
    0, /* Home key */
    0, /* Up Arrow */
    0, /* Page Up */
    '-', 0, /* Left Arrow */
    0, 0, /* Right Arrow */
    '+', 0, /* 79 - End key */
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0, 0, /* F11 Key */
    0, /* F12 Key */
    0 /* All other keys are undefined */
};
const char capsScancodes[256] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
    '(', ')', '_', '+', 0, /* Backspace */
    0, /* Tab */
    'Q', 'W', 'E', 'R', /* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, /* Enter key */
    0, /* 29 - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
    '"', '~', 0, /* Left shift */
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', /* 49 */
    'M', '<', '>', '?', 0, /* Right shift */
    '*', 0, /* Alt */
    ' ', /* Space bar */
    0, /* Caps lock */
    0, /* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0, 0, /* < ... F10 */
    0, /* 69 - Num lock */
    0, /* Scroll Lock */
    0, /* Home key */
    0, /* Up Arrow */
    0, /* Page Up */
    '-', 0, /* Left Arrow */
    0, 0, /* Right Arrow */
    '+', 0, /* 79 - End key */
    0, /* Down Arrow */
    0, /* Page Down */
    0, /* Insert Key */
    0, /* Delete Key */
    0, 0, 0, 0, /* F11 Key */
    0, /* F12 Key */
    0 /* All other keys are undefined */
};
KeyboardBuffer keyboardBuffer = { 0, 0 };
bool leftShift = false;
bool rightShift = false;
bool shift = false;
bool caps = false;
char* typingBuffer = 0;
uint64_t typingCursor = 0;
uint64_t typingLength = 0;

void drawCharacter(char character, uint32_t x, uint32_t y, uint32_t colour)
{
    uint32_t* address = information.framebuffer + y * information.width + x;
    uint8_t* glyph = font->data + font->glyphSize * character;
    for (uint32_t y = 0; y < font->height; y++)
    {
        uint32_t width = font->width;
        while (width)
        {
            uint32_t amount = min(8, width);
            for (uint8_t x = 0; x < amount; x++)
            {
                if (*glyph & (0b10000000 >> x))
                {
                    *address = colour;
                }
                address++;
            }
            glyph++;
            width -= amount;
        }
        address += information.width - font->width;
    }
}

void terminalKeyboard()
{
    while (true)
    {
        while (keyboardBuffer.tail != keyboardBuffer.head)
        {
            switch (keyboardBuffer.buffer[keyboardBuffer.tail].scancode)
            {
                case KEY_LEFT_SHIFT:
                    leftShift = keyboardBuffer.buffer[keyboardBuffer.tail].pressed;
                    shift = leftShift || rightShift;
                    break;
                case KEY_RIGHT_SHIFT:
                    rightShift = keyboardBuffer.buffer[keyboardBuffer.tail].pressed;
                    shift = leftShift || rightShift;
                    break;
                case KEY_CAPS_LOCK:
                    if (keyboardBuffer.buffer[keyboardBuffer.tail].pressed)
                    {
                        caps = !caps;
                    }
                    break;
            }
            if (typingBuffer)
            {
                switch (keyboardBuffer.buffer[keyboardBuffer.tail].scancode)
                {
                    case KEY_BACKSPACE:
                        if (keyboardBuffer.buffer[keyboardBuffer.tail].pressed && typingCursor > 0)
                        {
                            typingCursor--;
                            put('\b');
                        }
                        break;
                    case KEY_ENTER:
                        if (keyboardBuffer.buffer[keyboardBuffer.tail].pressed)
                        {
                            typingBuffer[typingCursor] = '\0';
                            put('\n');
                            typingBuffer = 0;
                        }
                        break;
                    default:
                        if (keyboardBuffer.buffer[keyboardBuffer.tail].pressed && typingCursor < typingLength)
                        {
                            char character = (caps ? !shift : shift) ? capsScancodes[keyboardBuffer.buffer[keyboardBuffer.tail].scancode] : scancodes[keyboardBuffer.buffer[keyboardBuffer.tail].scancode];
                            if (character)
                            {
                                typingBuffer[typingCursor++] = character;
                                put(character);
                            }
                        }
                        break;
                }
            }
            keyboardBuffer.tail++;
        }
        yieldThread();
    }
}

void blinkThread()
{
    bool blink = false;
    uint64_t last = getFemtoseconds();
    while (true)
    {
        uint64_t femtoseconds = getFemtoseconds();
        if (femtoseconds - last >= FEMTOSECONDS_PER_SECOND / 2)
        {
            last = femtoseconds;
            if (!writing && !displayObtained)
            {
                drawCharacter('_', cursorX * fontWidth, cursorY * fontHeight, blink ? colours[colour] : 0);
                blink = !blink;
            }
        }
        yieldThread();
    }
}

void initTerminal()
{
    serialPrint("Setting up terminal");
    registerSyscall(PUT, put);
    registerSyscall(WRITE, write);
    registerSyscall(READ, read);
    serialPrint("Loading font");
    font = (PsfFile*)getFile("/naul/font.psf", 0);
    fontWidth = font->width + 1;
    fontHeight = font->height;
    serialPrint("Setting up terminal graphics");
    terminalWidth = information.width / fontWidth;
    terminalHeight = information.height / fontHeight;
    terminalPitch = terminalWidth * 2;
    serialPrint("Allocating back buffer");
    backBuffer = allocate(terminalWidth * terminalHeight * sizeof(char) * 2);
    serialPrint("Setting up back buffer");
    setMemory16((uint16_t*)backBuffer, 0x00FF, terminalWidth * terminalHeight);
    serialPrint("Creating blink thread");
    createThread(blinkThread);
    serialPrint("Registering keyboard handler");
    registerKeyboard(&keyboardBuffer);
    createThread(terminalKeyboard);
    serialPrint("Set up terminal");
}

void redrawTerminal()
{
    setMemory32(information.framebuffer, 0, information.width * information.height);
    uint64_t x = 0;
    uint64_t y = 0;
    char* buffer = backBuffer;
    for (uint64_t i = 0; i < terminalWidth * terminalHeight; i++)
    {
        if (*buffer != TERMINAL_NO_CHAR)
        {
            drawCharacter(*buffer, x * fontWidth, y * fontHeight, colours[*(buffer + 1)]);
        }
        buffer += 2;
        x++;
        if (x == terminalWidth)
        {
            y++;
            x = 0;
        }
    }
}

void drop()
{
    uint64_t x = 0;
    uint64_t y = 0;
    char* buffer = backBuffer;
    char* bufferDrop = backBuffer;
    for (uint64_t i = 0; i < terminalWidth * terminalHeight; i++)
    {
        if (*buffer != TERMINAL_NO_CHAR)
        {
            drawCharacter(*buffer, x * fontWidth, y * fontHeight, 0);
        }
        if (i >= terminalWidth)
        {
            *bufferDrop = *buffer;
            if (*bufferDrop != TERMINAL_NO_CHAR)
            {
                *(bufferDrop + 1) = *(buffer + 1);
                drawCharacter(*bufferDrop, x * fontWidth, (y - 1) * fontHeight, colours[*(bufferDrop + 1)]);
            }
            bufferDrop += 2;
        }
        buffer += 2;
        x++;
        if (x == terminalWidth)
        {
            y++;
            x = 0;
        }
    }
    setMemory16((uint16_t*)(backBuffer + (terminalHeight - 1) * terminalPitch), 0x00FF, terminalPitch / 2);
}

void put(char character)
{
    lock(&writing);
    drawCharacter('_', cursorX * fontWidth, cursorY * fontHeight, 0);
    if (character == '\b')
    {
        if (cursorX > 0)
        {
            cursorX--;
        }
        else
        {
            cursorY--;
            cursorX = terminalWidth - 1;
        }
        uint64_t location = cursorY * terminalPitch + cursorX * 2;
        drawCharacter(backBuffer[location], cursorX * fontWidth, cursorY * fontHeight, 0);
        backBuffer[location] = TERMINAL_NO_CHAR;
    }
    else if (character == '\n')
    {
        cursorX = 0;
        if (cursorY + 1 != terminalHeight)
        {
            cursorY++;
        }
        else
        {
            drop();
        }
    }
    else if (character == TERM_CLEAR)
    {
        uint64_t x = 0;
        uint64_t y = 0;
        char* buffer = backBuffer;
        for (uint64_t i = 0; i < terminalWidth * terminalHeight; i++)
        {
            if (*buffer != TERMINAL_NO_CHAR)
            {
                drawCharacter(*buffer, x * fontWidth, y * fontHeight, 0);
                *buffer = TERMINAL_NO_CHAR;
            }
            buffer += 2;
            x++;
            if (x == terminalWidth)
            {
                y++;
                x = 0;
            }
        }
        cursorX = 0;
        cursorY = 0;
    }
    else
    {
        drawCharacter(character, cursorX * fontWidth, cursorY * fontHeight, colours[colour]);
        uint64_t location = cursorY * terminalPitch + cursorX * 2;
        backBuffer[location] = character;
        backBuffer[location + 1] = colour;
        cursorX++;
        if (cursorX == terminalWidth)
        {
            cursorX = 0;
            if (cursorY + 1 != terminalHeight)
            {
                cursorY++;
            }
            else
            {
                drop();
            }
        }
    }
    drawCharacter('_', cursorX * fontWidth, cursorY * fontHeight, colours[colour]);
    unlock(&writing);
}

void write(const char* message)
{
    while (*message)
    {
        if (*message == TERMINAL_COLOUR)
        {
            message++;
            colour = *message++;
            drawCharacter('_', cursorX * fontWidth, cursorY * fontHeight, colours[colour]);
        }
        else
        {
            put(*message++);
        }
    }
}

void read(char* buffer, uint64_t length)
{
    typingCursor = 0;
    typingBuffer = getAddress(buffer);
    typingLength = length;
    while (typingBuffer)
    {
        yieldThread();
    }
}
