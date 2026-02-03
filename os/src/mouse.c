#include <mouse.h>
#include <serial.h>
#include <syscalls.h>
#include <allocator.h>
#include <idt.h>
#include <pic.h>
#include <io.h>
#include <paging.h>
#include <cpu.h>

typedef struct
{
    void* next;
    void* prev;
    MouseBuffer* buffer;
} MouseBufferElement;

uint8_t mouseCycle = 1;
uint8_t mouseBytes[3];
MouseBufferElement* mouseBuffers = 0;

void mouse()
{
    mouseBytes[mouseCycle] = inb(0x60);
    mouseCycle++;
    if (mouseCycle == 3)
    {
        mouseCycle = 0;
        int16_t x = mouseBytes[1] - ((mouseBytes[0] << 4) & 0x100);
        int16_t y = mouseBytes[2] - ((mouseBytes[0] << 3) & 0x100);
        bool left = mouseBytes[0] & 0b00000001;
        bool right = mouseBytes[0] & 0b00000010;
        MouseBufferElement* element = mouseBuffers;
        while (true)
        {
            if (element->buffer)
            {
                element->buffer->buffer[element->buffer->current].x = x;
                element->buffer->buffer[element->buffer->current].y = y;
                element->buffer->buffer[element->buffer->current].left = left;
                element->buffer->buffer[element->buffer->current].right = right;
                element->buffer->current++;
            }
            element = element->next;
            if (element == mouseBuffers)
            {
                break;
            }
        }
    }
    picAck(12);
}

__attribute__((naked)) void mouseInterrupt()
{
    pushRegisters();
    __asm__ volatile ("cld; call mouse");
    popRegisters();
    __asm__ volatile ("iretq");
}

void initMouse()
{
    serialPrint("Setting up PS/2 mouse");
    registerSyscall(23, registerMouse);
    registerSyscall(24, unregisterMouse);
    serialPrint("Allocating mouse buffers");
    mouseBuffers = allocate(sizeof(MouseBufferElement));
    mouseBuffers->next = mouseBuffers;
    mouseBuffers->prev = mouseBuffers;
    mouseBuffers->buffer = 0;
    serialPrint("Installing mouse IRQ");
    installIrq(12, mouseInterrupt);
    serialPrint("Unmasking interrupt");
    unmaskPic(12);
    serialPrint("Enabling interrupts for mouse");
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    uint8_t status = inb(0x60) | 2;
    outb(0x64, 0x60);
    outb(0x60, status);
    outb(0x64, 0xD4);
    outb(0x60, 0xF6);
    inb(0x60);
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
    inb(0x60);
    serialPrint("Set up PS/2 mouse");
}

void registerMouse(MouseBuffer* buffer)
{
    __asm__ volatile ("cli");
    MouseBufferElement* element = allocate(sizeof(MouseBufferElement));
    ((MouseBufferElement*)mouseBuffers->prev)->next = element;
    element->next = mouseBuffers;
    element->prev = mouseBuffers->prev;
    mouseBuffers->prev = element;
    element->buffer = getAddress(buffer);
    __asm__ volatile ("sti");
}

void unregisterMouse(MouseBuffer* buffer)
{
    __asm__ volatile ("cli");
    MouseBuffer* address = getAddress(buffer);
    MouseBufferElement* element = mouseBuffers;
    while (element)
    {
        if (element->buffer == address)
        {
            ((MouseBufferElement*)element->prev)->next = element->next;
            ((MouseBufferElement*)element->next)->prev = element->prev;
            unallocate(element);
            break;
        }
        element = element->next;
    }
    __asm__ volatile ("sti");
}
