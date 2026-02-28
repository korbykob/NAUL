#pragma once

#include <definitions.h>

void initIpc();

void registerListener(uint64_t (*handler)(uint64_t arg1), const char* name);

void unregisterListener(uint64_t (*handler)(uint64_t arg1));

uint64_t sendMessage(const char* name, uint64_t arg1);
