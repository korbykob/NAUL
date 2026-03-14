#pragma once

#include <definitions.h>

#define IDT_INTERRUPT_GATE 0xE

void initIdt();

void installIsr(uint8_t interrupt, uint8_t attributes, void (*handler)());

void installIrq(uint8_t interrupt, void (*handler)());
