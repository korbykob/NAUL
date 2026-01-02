#pragma once

#include <definitions.h>

#define femtosecondsPerSecond 1000000000000000

void initHpet(uint64_t hpetAddress);

uint64_t getFemtoseconds();
