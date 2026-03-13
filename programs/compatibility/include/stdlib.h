#pragma once

#include <allocator.h>
#include <str.h>

#define malloc(amount) allocate(amount)
#define free(pointer) unallocate(pointer)
#define atoi(string) fromString(string)
