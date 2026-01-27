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
#include <cpu.h>
#include <mem.h>

uint8_t* font = 0;
char* backBuffer = 0;
uint32_t terminalWidth = 0;
uint32_t terminalHeight = 0;
uint32_t terminalPitch = 0;
uint32_t cursorX = 0;
uint32_t cursorY = 0;
const uint32_t colours[] = { 0x989898, 0xFFFFFF, 0x0080FF, 0x008000 };
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
KeyboardBuffer keyboardBuffer;
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
    uint8_t* glyph = font + 32 + 64 * character;
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
        address += information.width - 16;
    }
}

void terminalKeyboard()
{
    while (true)
    {
        if (keyboardBuffer.current > 0)
        {
            for (uint64_t i = 0; i < keyboardBuffer.current; i++)
            {
                switch (keyboardBuffer.buffer[i].scancode)
                {
                    case 42:
                        leftShift = keyboardBuffer.buffer[i].pressed;
                        shift = leftShift || rightShift;
                        break;
                    case 54:
                        rightShift = keyboardBuffer.buffer[i].pressed;
                        shift = leftShift || rightShift;
                        break;
                    case 58:
                        if (keyboardBuffer.buffer[i].pressed)
                        {
                            caps = !caps;
                        }
                        break;
                }
                if (typingBuffer)
                {
                    switch (keyboardBuffer.buffer[i].scancode)
                    {
                        case 14:
                            if (keyboardBuffer.buffer[i].pressed && typingCursor > 0)
                            {
                                typingCursor--;
                                put('\b');
                            }
                            break;
                        case 28:
                            if (keyboardBuffer.buffer[i].pressed)
                            {
                                typingBuffer[typingCursor] = '\0';
                                put('\n');
                                typingBuffer = 0;
                            }
                            break;
                        default:
                            if (keyboardBuffer.buffer[i].pressed && typingCursor < typingLength)
                            {
                                char character = (caps ? !shift : shift) ? capsScancodes[keyboardBuffer.buffer[i].scancode] : scancodes[keyboardBuffer.buffer[i].scancode];
                                if (character)
                                {
                                    typingBuffer[typingCursor++] = character;
                                    put(character);
                                }
                            }
                            break;
                    }
                }
            }
            keyboardBuffer.current = 0;
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
        if (femtoseconds - last >= femtosecondsPerSecond / 2)
        {
            last = femtoseconds;
            if (!writing)
            {
                drawCharacter('_', cursorX * 16, cursorY * 32, blink ? colours[colour] : 0);
                blink = !blink;
            }
        }
        yieldThread();
    }
}

void initTerminal()
{
    serialPrint("Setting up terminal");
    registerSyscall(2, put);
    registerSyscall(3, write);
    registerSyscall(4, clear);
    registerSyscall(5, read);
    serialPrint("Loading font");
    font = getFile("/naul/font.psf", 0);
    serialPrint("Setting up terminal graphics");
    terminalWidth = information.width / 16;
    terminalHeight = information.height / 32;
    terminalPitch = terminalWidth * 2;
    serialPrint("Allocating back buffer");
    backBuffer = allocate(terminalWidth * terminalHeight * sizeof(char) * 2);
    serialPrint("Setting up back buffer");
    setMemory16(backBuffer, '\0', terminalWidth * terminalHeight);
    serialPrint("Creating blink thread");
    createThread(blinkThread);
    serialPrint("Registering keyboard handler");
    keyboardBuffer.current = 0;
    registerKeyboard(&keyboardBuffer);
    createThread(terminalKeyboard);
    serialPrint("Set up terminal");
}

void drop()
{
    uint64_t x = 0;
    uint64_t y = 0;
    char* buffer = backBuffer;
    char* bufferDrop = backBuffer;
    for (uint64_t i = 0; i < terminalWidth * terminalHeight; i++)
    {
        if (*buffer != '\0')
        {
            drawCharacter(*buffer, x * 16, y * 32, 0);
        }
        if (i >= terminalWidth)
        {
            *bufferDrop = *buffer;
            if (*bufferDrop != '\0')
            {
                *(bufferDrop + 1) = *(buffer + 1);
                drawCharacter(*bufferDrop, x * 16, (y - 1) * 32, colours[*(bufferDrop + 1)]);
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
    setMemory8(backBuffer + (terminalHeight - 1) * terminalPitch, 0, terminalPitch);
}

void put(char character)
{
    lock(&writing);
    drawCharacter('_', cursorX * 16, cursorY * 32, 0);
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
        drawCharacter(backBuffer[location], cursorX * 16, cursorY * 32, 0);
        backBuffer[location] = '\0';
    }
    else
    {
        if (character != '\n')
        {
            drawCharacter(character, cursorX * 16, cursorY * 32, colours[colour]);
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
        else
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
    drawCharacter('_', cursorX * 16, cursorY * 32, colours[colour]);
    unlock(&writing);
}

void write(const char* message)
{
    while (*message)
    {
        if (*message == '\xff')
        {
            message++;
            colour = *message++;
            drawCharacter('_', cursorX * 16, cursorY * 32, colours[colour]);
        }
        else
        {
            put(*message++);
        }
    }
}

void clear()
{
    lock(&writing);
    drawCharacter('_', cursorX * 16, cursorY * 32, 0);
    uint64_t x = 0;
    uint64_t y = 0;
    char* buffer = backBuffer;
    for (uint64_t i = 0; i < terminalWidth * terminalHeight; i++)
    {
        if (*buffer != '\0')
        {
            drawCharacter(*buffer, x * 16, y * 32, 0);
            *buffer = '\0';
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
    unlock(&writing);
}

void read(char* buffer, uint64_t length)
{
    typingCursor = 0;
    typingBuffer = getAddress(buffer);
    typingLength = length;
    while (typingBuffer);
}
