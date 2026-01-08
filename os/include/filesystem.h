#pragma once

#include <definitions.h>

void initFilesystem();

bool checkFolder(const char* name);

bool checkFile(const char* name);

void createFolder(const char* name);

void* createFile(const char* name, uint64_t size);

const char** getFiles(const char* root, uint64_t* count);

uint8_t* getFile(const char* name, uint64_t* size);

void deleteFile(const char* name);
