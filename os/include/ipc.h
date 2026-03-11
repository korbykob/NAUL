#pragma once

#include <definitions.h>

void initIpc();

void registerListener(uint64_t (*handler)(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4), const char* name);

void unregisterListener(uint64_t (*handler)(uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4));

bool checkListener(const char* name);

uint64_t sendMessage(const char* name, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);
