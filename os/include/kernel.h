#pragma once

#include <definitions.h>
#include <bootloader.h>

void panic(uint8_t code);

void kernel(Info* information);
