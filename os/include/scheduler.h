#pragma once

#include <definitions.h>

void initScheduler();

void updateScheduler();

void destroyThread(uint64_t id);

uint64_t createThread(void (*function)());
