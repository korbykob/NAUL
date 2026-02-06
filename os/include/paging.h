#pragma once

#include <definitions.h>

#define PAGE_SIZE 0x200000

void initPaging();

uint64_t createTable(void* start, uint64_t pages);

void* getAddress(void* virtual);
