#pragma once

#include <definitions.h>
#include <bootloader.h>

void initFilesystem(InitFile* fileData, uint64_t fileCount);

void* createFile(const char* name, uint64_t size);

uint8_t* getFile(const char* name, uint64_t* size);

void deleteFile(const char* name);
