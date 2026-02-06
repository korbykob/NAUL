#pragma once

#include <definitions.h>

#define FEMTOSECONDS_PER_SECOND 1000000000000000
#define FEMTOSECONDS_PER_MILLISECOND 1000000000000
#define FEMTOSECONDS_PER_MICROSECOND 1000000000

void initHpet();

uint64_t getFemtoseconds();
