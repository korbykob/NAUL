#include <filesystem.h>
#include <bootloader.h>
#include <serial.h>
#include <allocator.h>
#include <scheduler.h>
#include <str.h>
#include <cpu.h>

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
    files = allocate(sizeof(File));
    files->next = files;
    files->prev = files;
    files->name = "";
    files->data = 0;
    serialPrint("Adding files to filesystem");
    for (uint64_t i = 0; i < information.fileCount; i++)
    {
        if (information.fileData[i].data != 0)
        {
            uint8_t* data = createFile(information.fileData[i].name, information.fileData[i].size);
            for (uint64_t j = 0; j < information.fileData[i].size; j++)
            {
                data[j] = information.fileData[i].data[j];
            }
        }
    }
    serialPrint("Set up filesystem");
}

bool checkFile(const char* name)
{
    lock(&managingFiles);
    File* file = files;
    while (file)
    {
        if (compareStrings(name, file->name) == 0)
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
    for (uint64_t i = 0; i < length; i++)
    {
        file->name[i] = name[i];
    }
    file->size = size;
    file->data = allocate(size);
    unlock(&managingFiles);
    return file->data;
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
