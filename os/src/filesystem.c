#include <filesystem.h>
#include <serial.h>
#include <syscalls.h>
#include <bootloader.h>
#include <allocator.h>
#include <scheduler.h>
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
bool managingFiles = false;

void initFilesystem()
{
    serialPrint("Setting up filesystem");
    registerSyscall(9, checkFolder);
    registerSyscall(10, checkFile);
    registerSyscall(11, createFolder);
    registerSyscall(12, createFile);
    registerSyscall(13, getFiles);
    registerSyscall(14, getFile);
    registerSyscall(15, deleteFile);
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
            copyMemory(information.fileData[i].data, data, information.fileData[i].size);
        }
    }
    serialPrint("Set up filesystem");
}

bool checkFolder(const char* name)
{
    lock(&managingFiles);
    File* file = files;
    while (file)
    {
        if (compareStrings(name, file->name) == 0 && file->data == 0)
        {
            unlock(&managingFiles);
            return true;
        }
        file = file->next;
        if (file == files)
        {
            break;
        }
    }
    unlock(&managingFiles);
    return false;
}

bool checkFile(const char* name)
{
    lock(&managingFiles);
    File* file = files;
    while (file)
    {
        if (compareStrings(name, file->name) == 0 && file->data)
        {
            unlock(&managingFiles);
            return true;
        }
        file = file->next;
        if (file == files)
        {
            break;
        }
    }
    unlock(&managingFiles);
    return false;
}

void createFolder(const char* name)
{
    lock(&managingFiles);
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
    unlock(&managingFiles);
}

void* createFile(const char* name, uint64_t size)
{
    lock(&managingFiles);
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
    unlock(&managingFiles);
    return file->data;
}

const char** getFiles(const char* root, uint64_t* count)
{
    lock(&managingFiles);
    *count = 0;
    uint64_t length = stringLength(root);
    File* file = files;
    while (file)
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
    while (file)
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
    unlock(&managingFiles);
    return items;
}

uint8_t* getFile(const char* name, uint64_t* size)
{
    lock(&managingFiles);
    File* file = files;
    while (file)
    {
        if (compareStrings(name, file->name) == 0)
        {
            if (size)
            {
                *size = file->size;
            }
            unlock(&managingFiles);
            return file->data;
        }
        file = file->next;
    }
    unlock(&managingFiles);
    return 0;
}

void deleteFile(const char* name)
{
    lock(&managingFiles);
    File* file = files;
    while (file)
    {
        if (compareStrings(name, file->name) == 0)
        {
            ((File*)file->prev)->next = file->next;
            ((File*)file->next)->prev = file->prev;
            unallocate(file->name);
            unallocate(file->data);
            unallocate(file);
            break;
        }
        file = file->next;
    }
    unlock(&managingFiles);
}
