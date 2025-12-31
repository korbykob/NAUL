#pragma once

#include <definitions.h>

void initAllocator(uint64_t end);

void markUnusable(uint64_t start, uint64_t end);

void* allocate(uint64_t amount);

void* allocateAligned(uint64_t amount, uint64_t alignment);

void unallocate(void* pointer);
