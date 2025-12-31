#pragma once

#include <definitions.h>

void initSyscalls();

void registerSyscall(uint64_t code, void* handler);
