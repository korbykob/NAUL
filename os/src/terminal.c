#include <terminal.h>
#include <serial.h>
#include <allocator.h>
#include <scheduler.h>
#include <syscalls.h>
#include <paging.h>
#include <keyboard.h>
#include <graphics.h>
#include <cpu.h>

uint32_t* videoBuffer = 0;
uint32_t width = 0;
uint32_t height = 0;
uint32_t terminalWidth = 0;
uint32_t terminalHeight = 0;
uint32_t cursorX = 0;
uint32_t cursorY = 0;
bool writing = false;
char scancodes[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', 0, /* Backspace */
    0, /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, /* Enter key */
    0, /* 29 - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
    '\'', '`',  0, /* Left shift */
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
char capsScancodes[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', /* 9 */
    '(', ')', '_', '+', 0, /* Backspace */
    0, /* Tab */
    'Q', 'W', 'E', 'R', /* 19 */
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0, /* Enter key */
    0, /* 29 - Control */
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', /* 39 */
    '"', '~',  0, /* Left shift */
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

void terminalKeyboard()
{
    while (true)
    {
        while (keyboardBuffer.current)
        {
            switch (keyboardBuffer.buffer[keyboardBuffer.current - 1].scancode)
            {
                case 42:
                    leftShift = keyboardBuffer.buffer[keyboardBuffer.current - 1].pressed;
                    shift = leftShift || rightShift;
                    break;
                case 54:
                    rightShift = keyboardBuffer.buffer[keyboardBuffer.current - 1].pressed;
                    shift = leftShift || rightShift;
                    break;
                case 58:
                    if (keyboardBuffer.buffer[keyboardBuffer.current - 1].pressed)
                    {
                        caps = !caps;
                    }
                    break;
            }
            if (typingBuffer)
            {
                switch (keyboardBuffer.buffer[keyboardBuffer.current - 1].scancode)
                {
                    case 14:
                        if (keyboardBuffer.buffer[keyboardBuffer.current - 1].pressed && typingCursor > 0)
                        {
                            typingCursor--;
                            put('\b');
                        }
                        break;
                    case 28:
                        if (keyboardBuffer.buffer[keyboardBuffer.current - 1].pressed)
                        {
                            typingBuffer[typingCursor] = '\0';
                            put('\n');
                            typingBuffer = 0;
                        }
                        break;
                    default:
                        if (keyboardBuffer.buffer[keyboardBuffer.current - 1].pressed)
                        {
                            char character = (caps ? !shift : shift) ? capsScancodes[keyboardBuffer.buffer[keyboardBuffer.current - 1].scancode] : scancodes[keyboardBuffer.buffer[keyboardBuffer.current - 1].scancode];
                            if (character)
                            {
                                typingBuffer[typingCursor++] = character;
                                put(character);
                            }
                        }
                        break;
                }
            }
            keyboardBuffer.current--;
        }
    }
}

void initTerminal(uint32_t* buffer, uint32_t screenX, uint32_t screenY, uint8_t* font)
{
    serialPrint("Setting up terminal");
    registerSyscall(0, put);
    registerSyscall(1, write);
    registerSyscall(2, clear);
    registerSyscall(3, read);
    serialPrint("Setting up terminal parameters");
    videoBuffer = buffer;
    width = screenX;
    height = screenY;
    serialPrint("Setting up terminal graphics");
    initGraphics(videoBuffer, width, font);
    terminalWidth = width / 16;
    terminalHeight = height / 32;
    drawCharacter('_', 0, 0, 0);
    serialPrint("Registering keyboard handler");
    keyboardBuffer.current = 0;
    registerKeyboard(&keyboardBuffer);
    createThread(terminalKeyboard);
    serialPrint("Set up terminal");
}

void drop()
{
    uint64_t* to = (uint64_t*)videoBuffer;
    uint64_t* from = (uint64_t*)(videoBuffer + 32 * width);
    for (uint32_t y = 0; y < (width * (height - 32)) / 2; y++)
    {
        *to++ = *from++;
    }
    drawRectangle(0, height - 32, width, 32, 0);
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
        drawRectangle(cursorX * 16, cursorY * 32, 16, 32, 0);
    }
    else
    {
        if (character != '\n')
        {
            drawCharacter(character, cursorX * 16, cursorY * 32, 0x989898);
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
    drawCharacter('_', cursorX * 16, cursorY * 32, 0x989898);
    unlock(&writing);
}

void write(const char* message)
{
    const char* messageAddress = getAddress((void*)message);
    while (*messageAddress)
    {
        put(*messageAddress++);
    }
}

void clear()
{
    lock(&writing);
    fillScreen(videoBuffer, 0, width, height);
    cursorX = 0;
    cursorY = 0;
    unlock(&writing);
}

void read(char* buffer)
{
    typingCursor = 0;
    typingBuffer = getAddress(buffer);
    while (typingBuffer);
}
