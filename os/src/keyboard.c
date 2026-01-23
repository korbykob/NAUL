#include <keyboard.h>
#include <serial.h>
#include <allocator.h>
#include <idt.h>
#include <pic.h>
#include <io.h>
#include <cpu.h>

typedef struct
{
    void* next;
    void* prev;
    KeyboardBuffer* buffer;
} KeyboardBufferElement;
KeyboardBufferElement* keyboardBuffers = 0;

void keyboard()
{
    uint8_t scancode = inb(0x60);
    bool unpressed = scancode & 0b10000000;
    if (unpressed)
    {
        scancode &= 0b01111111;
    }
    KeyboardBufferElement* element = keyboardBuffers;
    while (true)
    {
        element->buffer->buffer[element->buffer->current].scancode = scancode;
        element->buffer->buffer[element->buffer->current].pressed = !unpressed;
        element->buffer->current++;
        element = element->next;
        if (element == keyboardBuffers)
        {
            break;
        }
    }
    picAck(1);
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
    keyboardBuffers = allocate(sizeof(KeyboardBufferElement));
    keyboardBuffers->next = keyboardBuffers;
    keyboardBuffers->prev = keyboardBuffers;
    keyboardBuffers->buffer = 0;
    serialPrint("Installing keyboard IRQ");
    installIrq(1, keyboardInterrupt);
    serialPrint("Unmasking interrupt");
    unmaskPic(1);
    serialPrint("Flushing PS/2 input buffer");
    if (inb(0x64) & 0x01)
    {
        inb(0x60);
    }
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
    element->buffer = buffer;
    __asm__ volatile ("sti");
}

void unregisterKeyboard(KeyboardBuffer* buffer)
{
    __asm__ volatile ("cli");
    KeyboardBufferElement* element = keyboardBuffers;
    while (element)
    {
        if (element->buffer == buffer)
        {
            ((KeyboardBufferElement*)element->prev)->next = element->next;
            ((KeyboardBufferElement*)element->next)->prev = element->prev;
            unallocate(element);
            break;
        }
        element = element->next;
    }
    __asm__ volatile ("sti");
}
