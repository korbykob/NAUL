#pragma once

#include <definitions.h>

void initIdt();

void installIsr(uint8_t interrupt, void (*handler)());

void installIrq(uint8_t interrupt, void (*handler)());
