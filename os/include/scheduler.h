#pragma once

#include <definitions.h>

void initScheduler();

void updateScheduler();

uint64_t createThread(void (*function)());

void destroyThread(uint64_t id);

void exitThread();

