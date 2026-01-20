#pragma once

#include <definitions.h>

void initPaging();

uint64_t createTable(void* start, uint64_t pages);

void* getAddress(void* virtual);
