#include <mouse.h>
#include <serial.h>
#include <syscalls.h>
#include <allocator.h>
#include <idt.h>
#include <pic.h>
#include <io.h>
#include <paging.h>
#include <calls.h>
#include <cpu.h>

#define MOUSE_COMMAND 0x64
#define MOUSE_DATA 0x60
#define MOUSE_OVERFLOW_MASK 0x100
#define MOUSE_LEFT_MASK 0b1
#define MOUSE_RIGHT_MASK 0b10
#define MOUSE_INTERRUPT 12
#define PS2_ENABLE_SECOND 0xA8
#define PS2_GET_CONFIG 0x20
#define PS2_SECOND_INTERRUPT 0x2
#define PS2_SET_CONFIG 0x60
#define MOUSE_PS2_PORT 0xD4
#define MOUSE_DEFAULTS 0xF6
#define MOUSE_STREAMING 0xF4

typedef struct
{
    void* next;
    void* prev;
    MouseBuffer* buffer;
} MouseBufferElement;

uint8_t mouseCycle = 2;
uint8_t mouseBytes[3];
MouseBufferElement* mouseBuffers = 0;

void mouse()
{
    mouseBytes[mouseCycle] = inb(MOUSE_DATA);
    mouseCycle++;
    if (mouseCycle == 3)
    {
        mouseCycle = 0;
        int16_t x = mouseBytes[1] - ((mouseBytes[0] << 4) & MOUSE_OVERFLOW_MASK);
        int16_t y = mouseBytes[2] - ((mouseBytes[0] << 3) & MOUSE_OVERFLOW_MASK);
        bool left = mouseBytes[0] & MOUSE_LEFT_MASK;
        bool right = mouseBytes[0] & MOUSE_RIGHT_MASK;
        MouseBufferElement* element = mouseBuffers;
        while (true)
        {
            if (element->buffer)
            {
                element->buffer->buffer[element->buffer->head].x = x;
                element->buffer->buffer[element->buffer->head].y = y;
                element->buffer->buffer[element->buffer->head].left = left;
                element->buffer->buffer[element->buffer->head].right = right;
                element->buffer->head++;
            }
            element = element->next;
            if (element == mouseBuffers)
            {
                break;
            }
        }
    }
    picAck(MOUSE_INTERRUPT);
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
    registerSyscall(REGISTER_MOUSE, registerMouse);
    registerSyscall(UNREGISTER_MOUSE, unregisterMouse);
    serialPrint("Allocating mouse buffers");
    mouseBuffers = allocate(sizeof(MouseBufferElement));
    mouseBuffers->next = mouseBuffers;
    mouseBuffers->prev = mouseBuffers;
    mouseBuffers->buffer = 0;
    serialPrint("Installing mouse IRQ");
    installIrq(MOUSE_INTERRUPT, mouseInterrupt);
    serialPrint("Enabling second PS/2 port");
    outb(MOUSE_COMMAND, PS2_ENABLE_SECOND);
    serialPrint("Enabling second PS/2 interrupt");
    outb(MOUSE_COMMAND, PS2_GET_CONFIG);
    uint8_t status = inb(MOUSE_DATA) | PS2_SECOND_INTERRUPT;
    outb(MOUSE_COMMAND, PS2_SET_CONFIG);
    outb(MOUSE_DATA, status);
    serialPrint("Setting mouse to defaults");
    outb(MOUSE_COMMAND, MOUSE_PS2_PORT);
    outb(MOUSE_DATA, MOUSE_DEFAULTS);
    inb(MOUSE_DATA);
    serialPrint("Enabling mouse interrupts");
    outb(MOUSE_COMMAND, MOUSE_PS2_PORT);
    outb(MOUSE_DATA, MOUSE_STREAMING);
    inb(MOUSE_DATA);
    serialPrint("Unmasking interrupt");
    unmaskPic(MOUSE_INTERRUPT);
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
