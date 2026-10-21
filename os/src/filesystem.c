#include <filesystem.h>
#include <terminal.h>
#include <syscalls.h>
#include <bootloader.h>
#include <allocator.h>
#include <calls.h>
#include <str.h>
#include <cpu.h>
#include <mem.h>
#include <list.h>

typedef struct {
    List header;
    char* name;
    uint64_t size;
    uint8_t* data;
} File;

List files = { &files, &files };
bool filesystemLock = false;

void initFilesystem()
{
    log("Setting up filesystem");
    registerSyscall(CHECK_FOLDER, checkFolder);
    registerSyscall(CHECK_FILE, checkFile);
    registerSyscall(CREATE_FOLDER, createFolder);
    registerSyscall(CREATE_FILE, createFile);
    registerSyscall(GET_FILES, getFiles);
    registerSyscall(GET_FILE, getFile);
    registerSyscall(DELETE_FILE, deleteFile);
    log("Creating root folder");
    createFolder("");
    log("Adding files to filesystem");
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
    log("Set up filesystem");
}

bool checkFolder(const char* name)
{
    lock(&filesystemLock);
    File* file = (File*)&files;
    while (iterateList(&files, (List**)&file))
    {
        if (compareStrings(name, file->name) == 0 && file->data == 0)
        {
            unlock(&filesystemLock);
            return true;
        }
    }
    unlock(&filesystemLock);
    return false;
}

bool checkFile(const char* name)
{
    lock(&filesystemLock);
    File* file = (File*)&files;
    while (iterateList(&files, (List**)&file))
    {
        if (compareStrings(name, file->name) == 0 && file->data)
        {
            unlock(&filesystemLock);
            return true;
        }
    }
    unlock(&filesystemLock);
    return false;
}

void createFolder(const char* name)
{
    File* file = allocate(sizeof(File));
    uint64_t length = stringLength(name) + 1;
    file->name = allocate(length);
    copyString(name, file->name);
    file->size = 0;
    file->data = 0;
    lock(&filesystemLock);
    linkList(&files, &file->header);
    unlock(&filesystemLock);
}

void* createFile(const char* name, uint64_t size)
{
    File* file = allocate(sizeof(File));
    uint64_t length = stringLength(name) + 1;
    file->name = allocate(length);
    copyString(name, file->name);
    file->size = size;
    file->data = allocate(size);
    lock(&filesystemLock);
    linkList(&files, &file->header);
    unlock(&filesystemLock);
    return file->data;
}

const char** getFiles(const char* root, uint64_t* count)
{
    *count = 0;
    uint64_t length = stringLength(root);
    lock(&filesystemLock);
    File* file = (File*)&files;
    while (iterateList(&files, (List**)&file))
    {
        if (compareStrings(file->name, root) != 0 && compareStart(file->name, root, length) == 0 && !stringContains(file->name + length + 1, '/'))
        {
            *count = *count + 1;
        }
    }
    const char** items = allocate(*count * sizeof(const char*));
    uint64_t i = 0;
    file = (File*)&files;
    while (iterateList(&files, (List**)&file))
    {
        if (compareStrings(file->name, root) != 0 && compareStart(file->name, root, length) == 0 && !stringContains(file->name + length + 1, '/'))
        {
            items[i] = file->name;
            i++;
        }
    }
    unlock(&filesystemLock);
    return items;
}

uint8_t* getFile(const char* name, uint64_t* size)
{
    lock(&filesystemLock);
    File* file = (File*)&files;
    while (iterateList(&files, (List**)&file))
    {
        if (compareStrings(name, file->name) == 0)
        {
            break;
        }
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
    File* file = (File*)&files;
    while (iterateList(&files, (List**)&file))
    {
        if (compareStrings(name, file->name) == 0)
        {
            break;
        }
    }
    unlinkList(&file->header);
    unlock(&filesystemLock);
    unallocate(file->name);
    unallocate(file->data);
    unallocate(file);
}
