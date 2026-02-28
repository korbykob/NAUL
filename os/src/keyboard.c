#include <keyboard.h>
#include <serial.h>
#include <syscalls.h>
#include <allocator.h>
#include <idt.h>
#include <pic.h>
#include <io.h>
#include <paging.h>
#include <calls.h>
#include <cpu.h>

#define KEYBOARD_COMMAND 0x64
#define KEYBOARD_DATA 0x60
#define KEYBOARD_PRESSED_MASK 0b10000000
#define KEYBOARD_BUFFER_FULL 0x01
#define KEYBOARD_INTERRUPT 1

typedef struct
{
    void* next;
    void* prev;
    KeyboardBuffer* buffer;
} KeyboardBufferElement;

KeyboardBufferElement* keyboardBuffers = 0;

void keyboard()
{
    uint8_t scancode = inb(KEYBOARD_DATA);
    bool unpressed = scancode & KEYBOARD_PRESSED_MASK;
    if (unpressed)
    {
        scancode &= ~KEYBOARD_PRESSED_MASK;
    }
    KeyboardBufferElement* element = keyboardBuffers;
    while (true)
    {
        if (element->buffer)
        {
            element->buffer->buffer[element->buffer->head].scancode = scancode;
            element->buffer->buffer[element->buffer->head].pressed = !unpressed;
            element->buffer->head++;
        }
        element = element->next;
        if (element == keyboardBuffers)
        {
            break;
        }
    }
    picAck(KEYBOARD_INTERRUPT);
}

__attribute__((naked)) void keyboardInterrupt()
{
    pushRegisters();
    __asm__ volatile ("cld; call keyboard");
    popRegisters();
    __asm__ volatile ("iretq");
}

void initKeyboard()
{
    serialPrint("Setting up PS/2 keyboard");
    registerSyscall(REGISTER_KEYBOARD, registerKeyboard);
    registerSyscall(UNREGISTER_KEYBOARD, unregisterKeyboard);
    serialPrint("Allocating keyboard buffers");
    keyboardBuffers = allocate(sizeof(KeyboardBufferElement));
    keyboardBuffers->next = keyboardBuffers;
    keyboardBuffers->prev = keyboardBuffers;
    keyboardBuffers->buffer = 0;
    serialPrint("Installing keyboard IRQ");
    installIrq(KEYBOARD_INTERRUPT, keyboardInterrupt);
    serialPrint("Flushing PS/2 input buffer");
    if (inb(KEYBOARD_COMMAND) & KEYBOARD_BUFFER_FULL)
    {
        inb(KEYBOARD_DATA);
    }
    serialPrint("Unmasking interrupt");
    unmaskPic(KEYBOARD_INTERRUPT);
    serialPrint("Set up PS/2 keyboard");
}

void registerKeyboard(KeyboardBuffer* buffer)
{
    __asm__ volatile ("cli");
    KeyboardBufferElement* element = allocate(sizeof(KeyboardBufferElement));
    ((KeyboardBufferElement*)keyboardBuffers->prev)->next = element;
    element->next = keyboardBuffers;
    element->prev = keyboardBuffers->prev;
    keyboardBuffers->prev = element;
    element->buffer = getAddress(buffer);
    __asm__ volatile ("sti");
}

void unregisterKeyboard(KeyboardBuffer* buffer)
{
    __asm__ volatile ("cli");
    KeyboardBuffer* address = getAddress(buffer);
    KeyboardBufferElement* element = keyboardBuffers;
    while (element->buffer != address)
    {
        element = element->next;
    }
    ((KeyboardBufferElement*)element->prev)->next = element->next;
    ((KeyboardBufferElement*)element->next)->prev = element->prev;
    unallocate(element);
    __asm__ volatile ("sti");
}
