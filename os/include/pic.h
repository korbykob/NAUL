#pragma once

#include <definitions.h>

void initPic();

void unmaskPic(uint8_t interrupt);

void maskPic(uint8_t interrupt);

void picAck(uint8_t interrupt);
