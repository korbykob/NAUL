#include <ipc.h>
#include <terminal.h>
#include <calls.h>
#include <syscalls.h>
#include <allocator.h>
#include <cpu.h>
#include <str.h>
#include <list.h>

typedef struct
{
    List header;
    char name[256];
    uint64_t paging;
    uint64_t (*handler)(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);
} ListenerElement;

List listeners = { &listeners, &listeners };
bool ipcLock = false;

void initIpc()
{
    log("Setting up IPC");
    registerSyscall(REGISTER_LISTENER, registerListener);
    registerSyscall(UNREGISTER_LISTENER, unregisterListener);
    registerSyscall(CHECK_LISTENER, checkListener);
    registerSyscall(SEND_MESSAGE, sendMessage);
    log("Set up IPC");
}

void registerListener(uint64_t (*handler)(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4), const char* name)
{
    ListenerElement* element = allocate(sizeof(ListenerElement));
    copyString(name, element->name);
    __asm__ volatile ("mov %%cr3, %0" : "=r"(element->paging));
    element->handler = handler;
    lock(&ipcLock);
    linkList(&listeners, &element->header);
    unlock(&ipcLock);
}

void unregisterListener(uint64_t (*handler)(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4))
{
    uint64_t paging = 0;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(paging));
    lock(&ipcLock);
    ListenerElement* element = (ListenerElement*)&listeners;
    while (iterateList(&listeners, (List**)&element))
    {
        if (element->paging == paging && element->handler == handler)
        {
            break;
        }
    }
    unlinkList(&element->header);
    unlock(&ipcLock);
    unallocate(element);
}

bool checkListener(const char* name)
{
    lock(&ipcLock);
    ListenerElement* element = (ListenerElement*)&listeners;
    while (iterateList(&listeners, (List**)&element))
    {
        if (compareStrings(name, element->name) == 0)
        {
            unlock(&ipcLock);
            return true;
        }
    }
    unlock(&ipcLock);
    return false;
}

uint64_t sendMessage(const char* name, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4)
{
    lock(&ipcLock);
    ListenerElement* element = (ListenerElement*)&listeners;
    while (iterateList(&listeners, (List**)&element))
    {
        if (compareStrings(element->name, name) == 0)
        {
            break;
        }
    }
    unlock(&ipcLock);
    uint64_t oldPaging = 0;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(oldPaging));
    __asm__ volatile ("mov %0, %%cr3" : : "r"(element->paging));
    uint64_t value = element->handler(arg1, arg2, arg3, arg4);
    __asm__ volatile ("mov %0, %%cr3" : : "r"(oldPaging));
    return value;
}
