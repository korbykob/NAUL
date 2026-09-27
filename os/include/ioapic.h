#pragma once

#include <definitions.h>
#include <scheduler.h>

#define IOAPIC_OFFSET 0x20
#define ioapicAck() *(uint32_t*)LAPIC_EOI_REGISTER = 0

void initIoapic();

void unmaskIoapic(uint8_t interrupt);
