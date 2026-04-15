#include <filesystem.h>
#include <serial.h>
#include <syscalls.h>
#include <bootloader.h>
#include <allocator.h>
#include <calls.h>
#include <str.h>
#include <cpu.h>
#include <mem.h>

typedef struct {
    void* next;
    void* prev;
    char* name;
    uint64_t size;
    uint8_t* data;
} File;

File* files = 0;
bool filesystemLock = false;

void initFilesystem()
{
    serialPrint("Setting up filesystem");
    registerSyscall(CHECK_FOLDER, checkFolder);
    registerSyscall(CHECK_FILE, checkFile);
    registerSyscall(CREATE_FOLDER, createFolder);
    registerSyscall(CREATE_FILE, createFile);
    registerSyscall(GET_FILES, getFiles);
    registerSyscall(GET_FILE, getFile);
    registerSyscall(DELETE_FILE, deleteFile);
    serialPrint("Allocating file list");
    files = allocate(sizeof(File));
    files->next = files;
    files->prev = files;
    files->name = "";
    files->data = 0;
    serialPrint("Adding files to filesystem");
    for (uint64_t i = 0; i < information.fileCount; i++)
    {
        if (information.fileData[i].data == 0)
        {
            createFolder(information.fileData[i].name);
        }
        else
        {
            uint8_t* data = createFile(information.fileData[i].name, information.fileData[i].size);
            copyMemory8(information.fileData[i].data, data, information.fileData[i].size);
        }
    }
    serialPrint("Set up filesystem");
}

bool checkFolder(const char* name)
{
    lock(&filesystemLock);
    File* file = files;
    while (true)
    {
        if (compareStrings(name, file->name) == 0 && file->data == 0)
        {
            unlock(&filesystemLock);
            return true;
        }
        file = file->next;
        if (file == files)
        {
            break;
        }
    }
    unlock(&filesystemLock);
    return false;
}

bool checkFile(const char* name)
{
    lock(&filesystemLock);
    File* file = files;
    while (true)
    {
        if (compareStrings(name, file->name) == 0 && file->data)
        {
            unlock(&filesystemLock);
            return true;
        }
        file = file->next;
        if (file == files)
        {
            break;
        }
    }
    unlock(&filesystemLock);
    return false;
}

void createFolder(const char* name)
{
    lock(&filesystemLock);
    File* file = allocate(sizeof(File));
    ((File*)files->prev)->next = file;
    file->next = files;
    file->prev = files->prev;
    files->prev = file;
    uint64_t length = stringLength(name) + 1;
    file->name = allocate(length);
    copyString(name, file->name);
    file->size = 0;
    file->data = 0;
    unlock(&filesystemLock);
}

void* createFile(const char* name, uint64_t size)
{
    lock(&filesystemLock);
    File* file = allocate(sizeof(File));
    ((File*)files->prev)->next = file;
    file->next = files;
    file->prev = files->prev;
    files->prev = file;
    uint64_t length = stringLength(name) + 1;
    file->name = allocate(length);
    copyString(name, file->name);
    file->size = size;
    file->data = allocate(size);
    unlock(&filesystemLock);
    return file->data;
}

const char** getFiles(const char* root, uint64_t* count)
{
    lock(&filesystemLock);
    *count = 0;
    uint64_t length = stringLength(root);
    File* file = files;
    while (true)
    {
        if (compareStrings(file->name, root) != 0 && compareStart(file->name, root, length) == 0 && !stringContains(file->name + length + 1, '/'))
        {
            *count = *count + 1;
        }
        file = file->next;
        if (file == files)
        {
            break;
        }
    }
    const char** items = allocate(*count * sizeof(const char*));
    uint64_t i = 0;
    file = files;
    while (true)
    {
        if (compareStrings(file->name, root) != 0 && compareStart(file->name, root, length) == 0 && !stringContains(file->name + length + 1, '/'))
        {
            items[i] = file->name;
            i++;
        }
        file = file->next;
        if (file == files)
        {
            break;
        }
    }
    unlock(&filesystemLock);
    return items;
}

uint8_t* getFile(const char* name, uint64_t* size)
{
    lock(&filesystemLock);
    File* file = files;
    while (compareStrings(name, file->name) != 0)
    {
        file = file->next;
    }
    if (size)
    {
        *size = file->size;
    }
    unlock(&filesystemLock);
    return file->data;
}

void deleteFile(const char* name)
{
    lock(&filesystemLock);
    File* file = files;
    while (compareStrings(name, file->name) != 0)
    {
        file = file->next;
    }
    ((File*)file->prev)->next = file->next;
    ((File*)file->next)->prev = file->prev;
    unallocate(file->name);
    unallocate(file->data);
    unallocate(file);
    unlock(&filesystemLock);
}
