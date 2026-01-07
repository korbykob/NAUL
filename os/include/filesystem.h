#pragma once

#include <definitions.h>

void initFilesystem();

bool checkFile(const char* name);

void* createFile(const char* name, uint64_t size);

uint8_t* getFile(const char* name, uint64_t* size);

void deleteFile(const char* name);
