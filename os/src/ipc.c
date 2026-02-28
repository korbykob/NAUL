#include <ipc.h>
#include <serial.h>
#include <calls.h>
#include <syscalls.h>
#include <allocator.h>
#include <str.h>

typedef struct
{
    void* next;
    void* prev;
    char name[256];
    uint64_t paging;
    uint64_t (*handler)(uint64_t arg1);
} ListenerElement;

ListenerElement* listeners = 0;

void initIpc()
{
    serialPrint("Setting up IPC");
    registerSyscall(REGISTER_LISTENER, registerListener);
    registerSyscall(UNREGISTER_LISTENER, unregisterListener);
    registerSyscall(SEND_MESSAGE, sendMessage);
    serialPrint("Allocating listeners");
    listeners = allocate(sizeof(ListenerElement));
    listeners->next = listeners;
    listeners->prev = listeners;
    listeners->name[0] = '\0';
    listeners->paging = 0;
    listeners->handler = 0;
    serialPrint("Set up IPC");
}

void registerListener(uint64_t (*handler)(uint64_t arg1), const char* name)
{
    __asm__ volatile ("cli");
    ListenerElement* element = allocate(sizeof(ListenerElement));
    ((ListenerElement*)listeners->prev)->next = element;
    element->next = listeners;
    element->prev = listeners->prev;
    listeners->prev = element;
    copyString(name, element->name);
    __asm__ volatile ("mov %%cr3, %0" : "=r"(element->paging));
    element->handler = handler;
    __asm__ volatile ("sti");
}

void unregisterListener(uint64_t (*handler)(uint64_t arg1))
{
    __asm__ volatile ("cli");
    ListenerElement* element = listeners;
    while (element->handler != handler)
    {
        element = element->next;
    }
    ((ListenerElement*)element->prev)->next = element->next;
    ((ListenerElement*)element->next)->prev = element->prev;
    unallocate(element);
    __asm__ volatile ("sti");
}

uint64_t sendMessage(const char* name, uint64_t arg1)
{
    __asm__ volatile ("cli");
    ListenerElement* element = listeners;
    while (compareStrings(element->name, name) != 0)
    {
        element = element->next;
    }
    __asm__ volatile ("sti");
    uint64_t oldPaging = 0;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(oldPaging));
    __asm__ volatile ("mov %0, %%cr3" : : "r"(element->paging));
    uint64_t value = element->handler(arg1);
    __asm__ volatile ("mov %0, %%cr3" : : "r"(oldPaging));
    return value;
}
