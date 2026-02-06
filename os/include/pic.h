#pragma once

#include <definitions.h>

#define PIC_OFFSET 0x20

void initPic();

void unmaskPic(uint8_t interrupt);

void maskPic(uint8_t interrupt);

void picAck(uint8_t interrupt);
