#include <filesystem.h>
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

void initFilesystem(InitFile* fileData, uint64_t fileCount)
{
    serialPrint("Setting up filesystem");
    files = allocate(sizeof(File));
    files->next = files;
    files->prev = files;
    files->name = "";
    files->data = 0;
    serialPrint("Adding files to filesystem");
    for (uint64_t i = 0; i < fileCount; i++)
    {
        if (fileData[i].data != 0)
        {
            uint8_t* data = createFile(fileData[i].name, fileData[i].size);
            for (uint64_t j = 0; j < fileData[i].size; j++)
            {
                data[j] = fileData[i].data[j];
            }
        }
    }
    serialPrint("Set up filesystem");
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
