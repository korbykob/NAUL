#pragma once

#include <definitions.h>

#define PROCESS_ADDRESS 0x8000000000

void initProcesses();

uint64_t execute(const char* filename);

void quit();
